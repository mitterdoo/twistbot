#include "Neat.h"

namespace Neat
{
	// snake_case for functions, variables, parameters, and members.
	// PascalCase for classes and structs.
	// SCREAMING_SNAKE_CASE for constants and static variables.
	// Braces on new lines.

	void Ecosystem::rank_globally()
	{
		std::vector<Genome> all_genomes;
		for (Species& species : this->species)
		{
			for (Genome& genome : species.genomes)
			{
				all_genomes.push_back(genome);
			}
		}

		std::sort(all_genomes.begin(), all_genomes.end(), [](Genome a, Genome b) { return a.fitness > b.fitness; });

		for (int i = 0; i < all_genomes.size(); i++)
		{
			all_genomes[i].global_rank = i;
		}
	}

	float Ecosystem::get_total_average_fitness()
	{
		float total = 0;
		for (Species& species : this->species)
		{
			total += species.average_fitness;
		}
		return total;
	}

	void Ecosystem::cull_species(bool cut_to_one)
	{
		for (Species& species : this->species)
		{
			std::sort(species.genomes.begin(), species.genomes.end(), [](Genome a, Genome b) { return a.fitness > b.fitness; });

			int remaining = std::max(1, (int)species.genomes.size() / 2);
			if (cut_to_one)
			{
				remaining = 1;
			}

			species.genomes.resize(remaining);
		}
	}

	void Ecosystem::remove_stale_species()
	{
		std::vector<Species> survived;
		for (Species& species : this->species)
		{
			std::sort(species.genomes.begin(), species.genomes.end(), [](Genome a, Genome b) { return a.fitness > b.fitness; });

			if (!species.genomes.empty() && species.genomes[0].fitness > species.top_fitness)
			{
				species.top_fitness = species.genomes[0].fitness;
				species.staleness = 0;
			}
			else
			{
				species.staleness++;
			}

			if (species.staleness < PARAMS.stale_species || species.top_fitness >= this->max_fitness)
			{
				survived.push_back(species);
			}
		}

		if (survived.empty())
		{
			return;
		}
		this->species = survived;
	}

	void Ecosystem::remove_weak_species()
	{
		std::vector<Species> survived;
		float total_average_fitness = this->get_total_average_fitness();
		for (Species& species : this->species)
		{
			float should_breed = floor(species.average_fitness / total_average_fitness * this->population);
			if (should_breed >= 1)
			{
				survived.push_back(species);
			}
		}
		this->species = survived;
	}

	void Ecosystem::add_genome(Genome genome)
	{
		for (Species& species : this->species)
		{
			if (Genome::is_same_species(genome, species.genomes[0]))
			{
				species.genomes.push_back(genome);
				return;
			}
		}

		Species new_species;
		new_species.genomes.push_back(genome);
		this->species.push_back(new_species);
	}

	void Ecosystem::new_generation()
	{
		CURRENT_ECOSYSTEM = this;
		this->cull_species(false);
		this->rank_globally();
		this->remove_stale_species();
		this->rank_globally();

		for (Species& species : this->species)
		{
			species.calculate_average_fitness();
		}

		this->remove_weak_species();

		float total_average_fitness = this->get_total_average_fitness();

		std::vector<Genome> children;
		for (Species& species : this->species)
		{
			float should_breed = floor(species.average_fitness / total_average_fitness * this->population) - 1;
			for (int i = 0; i < should_breed; i++)
			{
				children.push_back(species.breed_child());
			}
		}

		this->cull_species(true);

		while (children.size() + this->species.size() < this->population)
		{
			int index = rand() % this->species.size();
			children.push_back(this->species[index].breed_child());
		}

		for (Genome& child : children)
		{
			this->add_genome(child);
		}

		this->generation++;
	}

	void Ecosystem::calculate_fitness(float (*callback)(Genome& genome))
	{
		this->cur_genome = 0;
		this->cur_species = 0;
		for (int species_index = 0; species_index < this->species.size(); species_index++)
		{
			Species& species = this->species[species_index];
			this->cur_species = species_index;
			for (int genome_index = 0; genome_index < species.genomes.size(); genome_index++)
			{
				Genome& genome = species.genomes[genome_index];
				this->cur_genome = genome_index;
				this->cur_genome_object = &genome;
				
				genome.generate_network();

				float fitness = callback(genome);

				genome.fitness = fitness;
				if (fitness > this->max_fitness)
				{
					this->max_fitness = fitness;
				}
			}
		}
	}

	Ecosystem Ecosystem::init_ecosystem()
	{
		Ecosystem ecosystem;
		CURRENT_ECOSYSTEM = &ecosystem;

		for (int i = 0; i < PARAMS.population; i++)
		{
			Genome genome = Genome::basic_genome();
			ecosystem.add_genome(genome);
		}

		return ecosystem;
	}

	void Species::calculate_average_fitness()
	{
		float total = 0;
		for (Genome& genome : this->genomes)
		{
			total += genome.global_rank; // TODO: should this be genome.fitness?
		}
		this->average_fitness = total / this->genomes.size();
	}

	Genome Species::breed_child()
	{
		Genome child;
		if (((float)rand()) / RAND_MAX >= PARAMS.rates.crossover)
		{
			child = this->genomes[rand() % this->genomes.size()].copy();
		}
		else
		{
			Genome parent1 = this->genomes[rand() % this->genomes.size()];
			Genome parent2 = this->genomes[rand() % this->genomes.size()];
			child = Genome::crossover(parent1, parent2);
		}
		child.mutate();
		return child;
	}

	Genome::Genome()
	{
		mutation_rates = PARAMS.rates;
	}

	Genome Genome::copy()
	{
		Genome genome = Genome();

		genome.genes = this->genes;
		genome.mutation_rates = this->mutation_rates;
		genome.max_neuron = this->max_neuron;

		return genome;
	}

	void Genome::generate_network()
	{
		this->network.clear();
		for (int i = 0; i < PARAMS.input_count; i++)
		{
			this->network[i] = Neuron(i);
		}
		for (int i = 0; i < PARAMS.output_count; i++)
		{
			this->network[PARAMS.max_nodes + i] = Neuron(PARAMS.max_nodes + i);
		}

		std::sort(this->genes.begin(), this->genes.end(), [](Gene a, Gene b) { return a.output < b.output; });

		for (Gene& gene : this->genes)
		{
			if (gene.enabled)
			{
				if (this->network.find(gene.output) == this->network.end())
				{
					this->network[gene.output] = Neuron(gene.output);
				}
				Neuron& neuron = this->network[gene.output];
				neuron.incoming.push_back(gene);

				if (this->network.find(gene.input) == this->network.end())
				{
					this->network[gene.input] = Neuron(gene.input);
				}
				this->network[gene.input].outgoing.push_back(gene.output);

			}
		}
	}

	std::vector<float> Genome::evaluate_network(std::vector<float> inputs)
	{
		for (int i = 0; i < PARAMS.input_count; i++)
		{
			this->network[i].value = inputs[i];
		}

		for (auto& [id, neuron] : this->network)
		{
			float sum = 0;
			for (Gene& incoming_gene : neuron.incoming)
			{
				sum += incoming_gene.weight * this->network[incoming_gene.input].value;
			}
			if (neuron.incoming.size() > 0)
			{
				neuron.value = sigmoid(sum);
			}
		}

		std::vector<float> outputs;
		for (int i = 0; i < PARAMS.output_count; i++)
		{
			outputs.push_back(this->network[PARAMS.max_nodes + i].value);
		}
		return outputs;
	}

	int Genome::random_neuron(bool non_input)
	{
		std::vector<int> neurons;
		if (!non_input)
		{
			for (int i = 0; i < PARAMS.input_count; i++)
			{
				neurons.push_back(i);
			}
		}
		for (int i = 0; i < PARAMS.output_count; i++)
		{
			neurons.push_back(PARAMS.max_nodes + i);
		}
		for (Gene& gene : this->genes)
		{
			if (!non_input || gene.input >= PARAMS.input_count)
			{
				neurons.push_back(gene.input);
			}
			if (!non_input || gene.output >= PARAMS.input_count)
			{
				neurons.push_back(gene.output);
			}
		}
		return neurons[rand() % neurons.size()];
	}

	bool Genome::contains_link(Gene gene)
	{
		for (Gene& g : this->genes)
		{
			if (g.input == gene.input && g.output == gene.output)
			{
				return true;
			}
		}
		return false;
	}

	void Genome::mutate()
	{

		if (rand() % 2 == 0)
			this->mutation_rates.connections *= PARAMS.rates.rate_down;
		else
			this->mutation_rates.connections *= PARAMS.rates.rate_up;
			
		if (rand() % 2 == 0)
			this->mutation_rates.perturb *= PARAMS.rates.rate_down;
		else
			this->mutation_rates.perturb *= PARAMS.rates.rate_up;
			
		if (rand() % 2 == 0)
			this->mutation_rates.step *= PARAMS.rates.rate_down;
		else
			this->mutation_rates.step *= PARAMS.rates.rate_up;

			
		if (rand() % 2 == 0)
			this->mutation_rates.link *= PARAMS.rates.rate_down;
		else
			this->mutation_rates.link *= PARAMS.rates.rate_up;
			
		if (rand() % 2 == 0)
			this->mutation_rates.bias *= PARAMS.rates.rate_down;
		else
			this->mutation_rates.bias *= PARAMS.rates.rate_up;
			
		if (rand() % 2 == 0)
			this->mutation_rates.node *= PARAMS.rates.rate_down;
		else
			this->mutation_rates.node *= PARAMS.rates.rate_up;
			
		if (rand() % 2 == 0)
			this->mutation_rates.enable *= PARAMS.rates.rate_down;
		else
			this->mutation_rates.enable *= PARAMS.rates.rate_up;
			
		if (rand() % 2 == 0)
			this->mutation_rates.disable *= PARAMS.rates.rate_down;
		else
			this->mutation_rates.disable *= PARAMS.rates.rate_up;

			
			
		if (rand() % 2 == 0)
			this->mutation_rates.crossover *= PARAMS.rates.rate_down;
		else
			this->mutation_rates.crossover *= PARAMS.rates.rate_up;


		if (((float)rand()) / RAND_MAX < this->mutation_rates.connections)
		{
			this->mutate_point();
		}

		float p = this->mutation_rates.link;
		while (p > 0)
		{
			if (((float)rand()) / RAND_MAX < p)
			{
				this->mutate_link(false);
			}
			p -= 1;
		}

		p = this->mutation_rates.bias;
		while (p > 0)
		{
			if (((float)rand()) / RAND_MAX < p)
			{
				this->mutate_link(true);
			}
			p -= 1;
		}

		p = this->mutation_rates.node;
		while (p > 0)
		{
			if (((float)rand()) / RAND_MAX < p)
			{
				this->mutate_node();
			}
			p -= 1;
		}

		p = this->mutation_rates.enable;
		while (p > 0)
		{
			if (((float)rand()) / RAND_MAX < p)
			{
				this->mutate_enabled(true);
			}
			p -= 1;
		}

		p = this->mutation_rates.disable;
		while (p > 0)
		{
			if (((float)rand()) / RAND_MAX < p)
			{
				this->mutate_enabled(false);
			}
			p -= 1;
		}

	}

	Genome Genome::crossover(Genome g1, Genome g2)
	{
		if (g2.fitness > g1.fitness)
		{
			std::swap(g1, g2);
		}

		Genome child = Genome();
		std::map<int, Gene> innovations;
		for (Gene& gene : g2.genes)
		{
			innovations[gene.innovation] = gene;
		}

		for (Gene& gene1 : g1.genes)
		{
			Gene& gene2 = innovations[gene1.innovation];
			if (gene2.innovation != 0 && rand() % 2 == 0 && gene2.enabled)
			{
				child.genes.push_back(gene2.copy());
			}
			else
			{
				child.genes.push_back(gene1.copy());
			}
		}

		child.max_neuron = std::max(g1.max_neuron, g2.max_neuron);

		child.mutation_rates = g1.mutation_rates;

		return child;
	}

	float Genome::get_disjointness(Genome g1, Genome g2)
	{
		std::map<int, bool> innovations;
		for (Gene& gene : g1.genes)
		{
			innovations[gene.innovation] = true;
		}
		for (Gene& gene : g2.genes)
		{
			innovations[gene.innovation] = true;
		}


		float disjoint = 0;
		for (Gene& gene : g1.genes)
		{
			if (innovations.find(gene.innovation) == innovations.end())
			{
				disjoint++;
			}
		}
		for (Gene& gene : g2.genes)
		{
			if (innovations.find(gene.innovation) == innovations.end())
			{
				disjoint++;
			}
		}

		return disjoint / std::max(g1.genes.size(), g2.genes.size());
	}

	float Genome::get_delta_weights(Genome g1, Genome g2)
	{
		std::map<int, Gene> innovations;
		for (Gene& gene : g2.genes)
		{
			innovations[gene.innovation] = gene;
		}

		float delta = 0;
		int coincident = 0;
		for (Gene& gene : g1.genes)
		{
			if (innovations.find(gene.innovation) != innovations.end())
			{
				Gene& gene2 = innovations[gene.innovation];
				delta += abs(gene.weight - gene2.weight);
				coincident++;
			}
		}

		return delta / coincident;
	}

	bool Genome::is_same_species(Genome g1, Genome g2)
	{
		float disjoint = PARAMS.delta_disjoint * Genome::get_disjointness(g1, g2);
		float delta_weights = PARAMS.delta_weights * Genome::get_delta_weights(g1, g2);
		return disjoint + delta_weights < PARAMS.delta_threshold;
	}

	Genome Genome::basic_genome()
	{
		Genome genome;
		if (PARAMS.start_with_input)
		{
			genome.max_neuron = 0;
			for (int i = 0; i < PARAMS.input_count; i++)
			{
				for (int j = 0; j < PARAMS.output_count; j++)
				{
					genome.max_neuron++;
					Gene gene;
					gene.input = i;
					gene.output = PARAMS.max_nodes + j;
					gene.weight = PARAMS.default_weights[i];
					gene.enabled = true;
					gene.innovation = new_innovation();
					genome.genes.push_back(gene);
				}
			}
		}
		else
		{
			genome.max_neuron = PARAMS.input_count;
		}
		genome.mutate();
		return genome;
	}

	void Genome::mutate_point()
	{
		for (Gene& gene : this->genes)
		{
			if (((float)rand()) / RAND_MAX < this->mutation_rates.perturb)
			{
				gene.weight += ((float)rand()) / RAND_MAX * this->mutation_rates.step * 2 - this->mutation_rates.step;
			}
			else
			{
				gene.weight = ((float)rand()) / RAND_MAX * 2 - 1;
			}
		}
	}

	void Genome::mutate_link(bool force_bias)
	{
		int neuron1 = this->random_neuron(false);
		int neuron2 = this->random_neuron(true);

		if (neuron1 >= PARAMS.max_nodes || neuron1 == neuron2 || (neuron1 < PARAMS.input_count && neuron2 < PARAMS.input_count))
		{
			return;
		}
		
		if (neuron2 < PARAMS.input_count)
		{
			std::swap(neuron1, neuron2);
		}

		Gene new_gene;
		new_gene.input = neuron1;
		new_gene.output = neuron2;
		if (force_bias)
		{
			new_gene.input = PARAMS.input_count;
		}
		if (this->contains_link(new_gene))
		{
			return;
		}
		new_gene.innovation = new_innovation();
		new_gene.weight = ((float)rand()) / RAND_MAX * 2 - 1;
		new_gene.enabled = true;
		this->genes.push_back(new_gene);
	}

	void Genome::mutate_node()
	{
		if (this->genes.empty())
		{
			return;
		}

		Gene gene = this->genes[rand() % this->genes.size()];
		if (!gene.enabled)
		{
			return;
		}
		this->max_neuron++;
		gene.enabled = false;

		Gene gene1 = gene.copy();
		gene1.output = this->max_neuron;
		gene1.weight = 1;
		gene1.innovation = new_innovation();
		gene1.enabled = true;
		this->genes.push_back(gene1);

		Gene gene2 = gene.copy();
		gene2.input = this->max_neuron;
		gene2.innovation = new_innovation();
		gene2.enabled = true;
		this->genes.push_back(gene2);
	}

	void Genome::mutate_enabled(bool enabled)
	{
		std::vector<Gene> candidates;
		for (Gene& gene : this->genes)
		{
			if (gene.enabled != enabled)
			{
				candidates.push_back(gene);
			}
		}
		if (candidates.empty())
		{
			return;
		}
		candidates[rand() % candidates.size()].enabled = enabled;
	}

	Neuron::Neuron(int id)
	{
		this->id = id;
	}

	Neuron::Neuron()
	{
	}

	Gene Gene::copy()
	{
		Gene gene;
		gene.input = this->input;
		gene.output = this->output;
		gene.weight = this->weight;
		gene.enabled = this->enabled;
		gene.innovation = this->innovation;
		return gene;
	}

	int new_innovation()
	{
		if (CURRENT_ECOSYSTEM != NULL)
		{
			return ++CURRENT_ECOSYSTEM->innovation;
		}
		return -1;
	}

	float sigmoid(float x)
	{
		return 2.0 / (1 + exp(-SIGMOID_CONSTANT * x)) - 1;
	}

}
