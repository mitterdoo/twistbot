#include "Neat.h"

namespace Neat
{
	// snake_case for functions, variables, parameters, and members.
	// PascalCase for classes and structs.
	// SCREAMING_SNAKE_CASE for constants and static variables.
	// Braces on new lines.

	Gene::ptr Gene::copy()
	{
		Gene::ptr gene = std::make_shared<Gene>();
		gene->input = this->input;
		gene->output = this->output;
		gene->weight = this->weight;
		gene->enabled = this->enabled;
		gene->innovation = this->innovation;
		return gene;
	}

	Neuron::Neuron(int id)
	{
		this->id = id;
	}

	Neuron::Neuron()
	{
	}

	

	Genome::Genome()
	{
		mutation_rates = PARAMS.rates;
	}

	Genome::ptr Genome::copy()
	{
		Genome::ptr genome = std::make_shared<Genome>();

		for (Gene::ptr gene : this->genes)
		{
			genome->genes.push_back(gene->copy());
		}
		genome->mutation_rates = this->mutation_rates;
		genome->max_neuron = this->max_neuron;

		return genome;
	}

	void Genome::generate_network()
	{
		this->network.clear();
		for (int i = 0; i < PARAMS.input_count; i++)
		{
			this->network[i] = std::make_shared<Neuron>(i);
		}
		for (int i = 0; i < PARAMS.output_count; i++)
		{
			this->network[PARAMS.max_nodes + i] = std::make_shared<Neuron>(PARAMS.max_nodes + i);
		}

		std::sort(this->genes.begin(), this->genes.end(), [](Gene::ptr a, Gene::ptr b) { return a->output < b->output; });

		for (Gene::ptr gene : this->genes)
		{
			if (gene->enabled)
			{
				if (this->network.find(gene->output) == this->network.end())
				{
					this->network[gene->output] = std::make_shared<Neuron>(gene->output);
				}
				Neuron::ptr neuron = this->network[gene->output];
				neuron->incoming.push_back(gene);

				if (this->network.find(gene->input) == this->network.end())
				{
					this->network[gene->input] = std::make_shared<Neuron>(gene->input);
				}
				this->network[gene->input]->outgoing.push_back(gene->output);

			}
		}
	}

	std::vector<float> Genome::evaluate_network(std::vector<float> inputs)
	{
		for (int i = 0; i < PARAMS.input_count; i++)
		{
			this->network[i]->value = inputs[i];
		}

		for (auto& [id, neuron] : this->network)
		{
			float sum = 0;
			for (Gene::ptr incoming_gene : neuron->incoming)
			{
				sum += incoming_gene->weight * this->network[incoming_gene->input]->value;
			}
			if (neuron->incoming.size() > 0)
			{
				neuron->value = sigmoid(sum);
			}
		}

		std::vector<float> outputs;
		for (int i = 0; i < PARAMS.output_count; i++)
		{
			outputs.push_back(this->network[PARAMS.max_nodes + i]->value);
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
		for (Gene::ptr gene : this->genes)
		{
			if (!non_input || gene->input >= PARAMS.input_count)
			{
				neurons.push_back(gene->input);
			}
			if (!non_input || gene->output >= PARAMS.input_count)
			{
				neurons.push_back(gene->output);
			}
		}
		return neurons[rand() % neurons.size()];
	}

	Gene::ptr Genome::contains_link(Gene::ptr gene)
	{
		for (Gene::ptr g : this->genes)
		{
			if (g->input == gene->input && g->output == gene->output)
			{
				return g;
			}
		}
		return nullptr;
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

	Genome::ptr Genome::crossover(Genome::ptr g1, Genome::ptr g2)
	{
		if (g2->fitness > g1->fitness)
		{
			// g1 should be the fittest
			std::swap(g1, g2);
		}
		Genome::ptr child = std::make_shared<Genome>();

		// allows identifying disjoint vs excess genes
		//int highest_common_innovation = -1;
		
		std::map<int, Gene::ptr> innovations;
		std::map<int, bool> innovation_is_from_fittest_gene;
		std::vector<Gene::ptr> non_matching;
		Gene::ptr found;
		for (Gene::ptr gene : g1->genes)
		{
			innovation_is_from_fittest_gene[gene->innovation] = true;
			innovations[gene->innovation] = gene;
			if ((found = g2->contains_link(gene)) != nullptr)
			{
				//highest_common_innovation = gene->innovation;
				// matching genes are inherited randomly
				if (rand() % 2 == 0)
				{
					child->genes.push_back(gene->copy());
				}
				else
				{
					child->genes.push_back(found->copy());
				}
			}
			else
			{
				non_matching.push_back(gene);
			}
		}

		for (Gene::ptr gene : non_matching)
		{
			if (innovation_is_from_fittest_gene.find(gene->innovation) != innovation_is_from_fittest_gene.end())
			{
				// disjoint genes are inherited from the fittest parent
				child->genes.push_back(gene->copy());
			}
		}

		/*
		std::map<int, Gene::ptr> innovations;
		for (Gene::ptr gene : g2->genes)
		{
			innovations[gene->innovation] = gene;
		}

		for (Gene::ptr gene1 : g1->genes)
		{
			Gene::ptr gene2 = innovations[gene1->innovation];
			if (gene2 != NULL && rand() % 2 == 0 && gene2->enabled)
			{
				child->genes.push_back(gene2->copy());
			}
			else
			{
				child->genes.push_back(gene1->copy());
			}
		}

		*/

		child->max_neuron = std::max(g1->max_neuron, g2->max_neuron);

		child->mutation_rates = g1->mutation_rates;

		return child;
	}

	float Genome::get_disjointness(Genome::ptr g1, Genome::ptr g2)
	{
		std::map<int, bool> innovations1;
		std::map<int, bool> innovations2;
		for (Gene::ptr gene : g1->genes)
		{
			innovations1[gene->innovation] = true;
		}
		for (Gene::ptr gene : g2->genes)
		{
			innovations2[gene->innovation] = true;
		}


		float disjoint = 0;
		for (Gene::ptr gene : g1->genes)
		{
			if (innovations2.find(gene->innovation) == innovations2.end())
			{
				disjoint++;
			}
		}
		for (Gene::ptr gene : g2->genes)
		{
			if (innovations1.find(gene->innovation) == innovations1.end())
			{
				disjoint++;
			}
		}

		return disjoint / std::max(g1->genes.size(), g2->genes.size());
	}

	float Genome::get_delta_weights(Genome::ptr g1, Genome::ptr g2)
	{
		std::map<int, Gene::ptr> innovations;
		for (Gene::ptr gene : g2->genes)
		{
			innovations[gene->innovation] = gene;
		}

		float delta = 0;
		int coincident = 0;
		for (Gene::ptr gene : g1->genes)
		{
			if (innovations.find(gene->innovation) != innovations.end())
			{
				Gene::ptr gene2 = innovations[gene->innovation];
				delta += abs(gene->weight - gene2->weight);
				coincident++;
			}
		}

		return delta / coincident;
	}

	bool Genome::is_same_species(Genome::ptr g1, Genome::ptr g2)
	{
		if (g1->genes.empty() && g2->genes.empty())
		{
			return true;
		}

		float disjoint = PARAMS.delta_disjoint * Genome::get_disjointness(g1, g2);
		float delta_weights = PARAMS.delta_weights * Genome::get_delta_weights(g1, g2);
		return disjoint + delta_weights < PARAMS.delta_threshold;
	}

	Genome::ptr Genome::basic_genome()
	{
		Genome::ptr genome = std::make_shared<Genome>();
		if (PARAMS.start_with_input)
		{
			genome->max_neuron = 0;
			//for (int i = 0; i < PARAMS.input_count; i++)
			//{
			int i = PARAMS.input_count - 1; // bias node
			for (int j = 0; j < PARAMS.output_count; j++)
			{
				genome->max_neuron++;
				Gene::ptr gene = std::make_shared<Gene>();
				gene->input = i;
				gene->output = PARAMS.max_nodes + j;
				gene->weight = ((float)(rand() % RAND_MAX)) / RAND_MAX * 2 - 1;
				gene->enabled = true;
				gene->innovation = j;
				genome->genes.push_back(gene);
			}
			//}
		}
		else
		{
			genome->max_neuron = PARAMS.input_count;
		}
		//genome->mutate();
		return genome;
	}

	void Genome::mutate_point()
	{
		for (Gene::ptr gene : this->genes)
		{
			if (((float)rand()) / RAND_MAX < this->mutation_rates.perturb)
			{
				gene->weight += ((float)rand()) / RAND_MAX * this->mutation_rates.step * 2 - this->mutation_rates.step;
			}
			else
			{
				gene->weight = ((float)rand()) / RAND_MAX * 2 - 1;
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

		Gene::ptr new_gene = std::make_shared<Gene>();
		new_gene->input = neuron1;
		new_gene->output = neuron2;
		if (force_bias)
		{
			new_gene->input = PARAMS.input_count;
		}
		if (this->contains_link(new_gene) != nullptr)
		{
			return;
		}
		new_gene->innovation = new_innovation(new_gene->input, new_gene->output);
		new_gene->weight = ((float)rand()) / RAND_MAX * 2 - 1;
		new_gene->enabled = true;
		this->genes.push_back(new_gene);
	}

	void Genome::mutate_node()
	{
		if (this->genes.empty())
		{
			return;
		}

		Gene::ptr gene = this->genes[rand() % this->genes.size()];
		if (!gene->enabled)
		{
			return;
		}
		this->max_neuron++;
		gene->enabled = false;

		Gene::ptr gene1 = gene->copy();
		gene1->output = this->max_neuron;
		gene1->weight = 1;
		gene1->innovation = new_innovation(gene1->input, gene1->output);
		gene1->enabled = true;
		this->genes.push_back(gene1);

		Gene::ptr gene2 = gene->copy();
		gene2->input = this->max_neuron;
		gene2->innovation = new_innovation(gene2->input, gene2->output);
		gene2->enabled = true;
		this->genes.push_back(gene2);
	}

	void Genome::mutate_enabled(bool enabled)
	{
		std::vector<Gene::ptr> candidates;
		for (Gene::ptr gene : this->genes)
		{
			if (gene->enabled != enabled)
			{
				candidates.push_back(gene);
			}
		}
		if (candidates.empty())
		{
			return;
		}
		candidates[rand() % candidates.size()]->enabled = enabled;
	}

	std::map<int, Neuron::ptr> Genome::get_used_nodes()
	{
		std::map<int, Neuron::ptr> nodes;
		std::vector<Neuron::ptr> open;

		for (int i = 0; i < PARAMS.output_count; i++)
		{
			open.push_back(this->network[PARAMS.max_nodes + i]);
		}

		while (!open.empty())
		{
			Neuron::ptr neuron = open.back();
			open.pop_back();
			
			if (neuron->incoming.size() > 0 || neuron->id < PARAMS.input_count)
			{
				nodes[neuron->id] = neuron;
				for (Gene::ptr gene : neuron->incoming)
				{
					if (nodes.find(gene->input) == nodes.end())
					{
						open.push_back(this->network[gene->input]);
					}
				}
			}
			else if (neuron->incoming.size() == 0)
			{
				nodes[neuron->id] = neuron;
			}
		}

		return nodes;

	}

	void Ecosystem::rank_globally()
	{
		std::vector<Genome::ptr> all_genomes;
		for (Species::ptr species : this->species)
		{
			for (Genome::ptr genome : species->genomes)
			{
				all_genomes.push_back(genome);
			}
		}

		std::sort(all_genomes.begin(), all_genomes.end(), [](Genome::ptr a, Genome::ptr b) { return a->fitness > b->fitness; });

		for (int i = 0; i < all_genomes.size(); i++)
		{
			all_genomes[i]->global_rank = i + 1;
		}
	}

	float Ecosystem::get_total_average_fitness()
	{
		float total = 0;
		for (Species::ptr species : this->species)
		{
			total += species->average_fitness;
		}
		return total;
	}

	void Ecosystem::cull_species(bool cut_to_one)
	{
		for (Species::ptr species : this->species)
		{
			std::sort(species->genomes.begin(), species->genomes.end(), [](Genome::ptr a, Genome::ptr b) { return a->fitness > b->fitness; });

			int remaining = std::max(1, (int)species->genomes.size() / 2);
			if (cut_to_one)
			{
				remaining = 1;
			}

			species->genomes.resize(remaining);
		}
	}

	void Ecosystem::remove_stale_species()
	{
		std::vector<Species::ptr> survived;
		for (Species::ptr species : this->species)
		{
			std::sort(species->genomes.begin(), species->genomes.end(), [](Genome::ptr a, Genome::ptr b) { return a->fitness > b->fitness; });

			if (!species->genomes.empty() && species->genomes[0]->fitness > species->top_fitness)
			{
				species->top_fitness = species->genomes[0]->fitness;
				species->staleness = 0;
			}
			else
			{
				species->staleness++;
			}

			if (species->staleness < PARAMS.stale_species_threshold || species->top_fitness >= this->max_fitness)
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
		std::vector<Species::ptr> survived;
		float total_average_fitness = this->get_total_average_fitness();
		for (Species::ptr species : this->species)
		{
			float should_breed = floor(species->average_fitness / total_average_fitness * this->population);
			if (should_breed >= 1)
			{
				survived.push_back(species);
			}
		}
		this->species = survived;
	}

	void Ecosystem::add_genome(Genome::ptr genome)
	{
		for (Species::ptr species : this->species)
		{
			if (Genome::is_same_species(genome, species->genomes[0]))
			{
				species->genomes.push_back(genome);
				return;
			}
		}

		Species::ptr new_species = std::make_shared<Species>();
		new_species->genomes.push_back(genome);
		this->species.push_back(new_species);
	}

	void Ecosystem::new_generation()
	{
		CURRENT_ECOSYSTEM = this;
		this->cull_species(false);
		this->rank_globally();
		this->remove_stale_species();
		this->rank_globally();

		for (Species::ptr species : this->species)
		{
			species->calculate_average_fitness();
		}

		this->remove_weak_species();

		float total_average_fitness = this->get_total_average_fitness();

		std::vector<Genome::ptr> children;
		for (Species::ptr species : this->species)
		{
			float should_breed = floor(species->average_fitness / total_average_fitness * this->population) - 1;
			for (int i = 0; i < should_breed; i++)
			{
				children.push_back(species->breed_child());
			}
		}

		this->cull_species(true);

		while (children.size() + this->species.size() < this->population)
		{
			int index = rand() % this->species.size();
			children.push_back(this->species[index]->breed_child());
		}

		for (Genome::ptr child : children)
		{
			this->add_genome(child);
		}

		this->generation++;

		int population_count = 0;
		for (Species::ptr species : this->species)
		{
			population_count += species->genomes.size();
		}
		printf("population: %d\n", population_count);
	}

	void Ecosystem::calculate_fitness(float (*callback)(Genome::ptr genome))
	{
		this->cur_genome = 0;
		this->cur_species = 0;
		for (int species_index = 0; species_index < this->species.size(); species_index++)
		{
			Species::ptr species = this->species[species_index];
			this->cur_species = species_index;
			for (int genome_index = 0; genome_index < species->genomes.size(); genome_index++)
			{
				Genome::ptr genome = species->genomes[genome_index];
				this->cur_genome = genome_index;
				this->cur_genome_object = genome;
				
				genome->generate_network();

				float fitness = callback(genome);

				genome->fitness = fitness;
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
			Genome::ptr genome = Genome::basic_genome();
			ecosystem.add_genome(genome);
		}

		return ecosystem;
	}

	void Species::calculate_average_fitness()
	{
		float total = 0;
		for (Genome::ptr genome : this->genomes)
		{
			total += genome->global_rank; // TODO: should this be genome.fitness?
		}
		this->average_fitness = total / this->genomes.size();
	}

	Genome::ptr Species::breed_child()
	{
		Genome::ptr child = std::make_shared<Genome>();
		if (((float)rand()) / RAND_MAX >= PARAMS.rates.crossover)
		{
			child = this->genomes[rand() % this->genomes.size()]->copy();
		}
		else
		{
			Genome::ptr parent1 = this->genomes[rand() % this->genomes.size()];
			Genome::ptr parent2 = this->genomes[rand() % this->genomes.size()];
			child = Genome::crossover(parent1, parent2);
		}
		child->mutate();
		return child;
	}


	int new_innovation(int input, int output)
	{
		if (CURRENT_ECOSYSTEM != NULL)
		{
			int hash = Neat::hash(input, output);
			if (CURRENT_ECOSYSTEM->innovations.find(hash) != CURRENT_ECOSYSTEM->innovations.end())
			{
				return CURRENT_ECOSYSTEM->innovations[hash];
			}
			else
			{
				CURRENT_ECOSYSTEM->innovation++;
				CURRENT_ECOSYSTEM->innovations[hash] = CURRENT_ECOSYSTEM->innovation;
				return CURRENT_ECOSYSTEM->innovation;
			}
		}
		return -1;
	}

	float sigmoid(float x)
	{
		//return 2.0 / (1 + exp(-SIGMOID_CONSTANT * x)) - 1;
		return 1.0 / (1 + exp(-SIGMOID_CONSTANT * x));
	}

	
	int hash(int input, int output)
	{
		return (input + 1) * (output + 1) - 1;
	}

}
