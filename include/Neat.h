#pragma once
#include <vector>
#include <map>
#include <algorithm>
#include <float.h>
#include <math.h>

namespace Neat
{

	/*
	
	Genome network:
	{

		0: input,
		1: input,
		2: input,
		...
		input_count + 0: bias (counts as an input),


		input_count + 1: new Gene,
		input_count + 2: new Gene,
		...
		max_nodes - 1: new Gene,


		max_nodes + 0: output,
		max_nodes + 1: output,
		max_nodes + 2: output

	}
	
	*/

	#define SIGMOID_CONSTANT 0.5

	class Params
	{
	public:
		static const int input_count = 4;
		float default_weights[input_count] = {
			0.1,
			0.1,
			0.1,
			0.1
		};

		int output_count = 1;
		int population = 70;

		int max_nodes = 1000000;

		float delta_disjoint = 2.0;
		float delta_weights = 0.4;
		float delta_threshold = 1.0;

		int stale_species = 15;

		bool start_with_input = false;

		struct Rates
		{
			// If probabilities are over 1, their action will be executed multiple times (subtracting 1 each time)
			float connections =	0.25; // Probability of mutating a genome's weights
			float perturb =		0.90; // Probability of slightly shifting a genome's weight. If fail, reassigns the weight instead.
			float step =		0.10; // Value to shift a weight by
			
			float link =		2.00; // Probability of linking a random pair of nodes together
			float bias = 		0.40; // Probability of linking a random node to the bias node
			float node =		0.50; // Probability of inserting a node between two nodes
			float enable =		0.20; // Probability of enabling a node
			float disable =		0.40; // Probability of disabling a node

			float rate_down =	0.99;
			float rate_up =		1.01;

			float crossover =	0.75;
		} rates;

	};
	
	const Params PARAMS;

	class Gene
	{
	public:
		int input;
		int output;
		float weight = 0;
		bool enabled = true;
		int innovation;

		/// @brief Creates a copy of a Gene
		/// @return The copied Gene
		Gene copy();
	};
	
	class Neuron
	{
	public:
		/// @brief List of other genes feeding into this one
		std::vector<Gene> incoming;
		/// @brief IDs of outgoing neurons
		std::vector<int> outgoing;
		float value = 0;
		int id = -1;

		Neuron(int id);
		Neuron();
	};

	typedef std::map<int, Neuron> Network;

	class Genome
	{
	public:
		std::vector<Gene> genes;
		float fitness = 0;
		float adjusted_fitness = 0;
		Network network;
		int max_neuron = 0;
		int global_rank = 0;

		Params::Rates mutation_rates;

		/// @brief Initializes a Genome with the current rates set in `PARAMS`
		Genome();

		/// @brief Creates a copy of a Genome
		/// @return The copied Genome
		Genome copy();
		
		/// @brief Generates the neural Network for a genome.
		void generate_network();

		/// @brief Evaluates a previously-generated network.
		/// @param inputs Input values
		/// @return Output values
		std::vector<float> evaluate_network(std::vector<float> inputs);

		/// @brief Picks a random neuron number out of the Genes
		/// @param ignore_input Whether Genes connected to an input shall be ignored.
		/// @return Random neuron, or 0 if a neuron couldn't be picked.
		int random_neuron(bool ignore_input);

		/// @brief Checks if any Gene is connected to a specific Gene
		/// @param gene The Gene to check
		/// @return Whether the Gene is connected
		bool contains_link(Gene gene);

		/// @brief Randomly mutates a Genome
		void mutate();


		/// @brief Crosses over two genomes and creates a child.
		/// @param g1 Genome
		/// @param g2 Genome
		/// @return The new child Genome
		static Genome crossover(Genome g1, Genome g2);

		/// @brief Returns how "disjoint" the Genes in two Genomes are.
		/// @param g1 Genome
		/// @param g2 Genome
		/// @return Disjointness
		static float get_disjointness(Genome g1, Genome g2);

		/// @brief Sums all the (unsigned) differences between each shared Gene and returns the average
		/// @param g1 Genome
		/// @param g2 Genome
		/// @return Average unsigned differences
		static float get_delta_weights(Genome g1, Genome g2);

		/// @brief Returns whether two Genomes may be classified within the same Species
		/// @param g1 Genome
		/// @param g2 Genome
		/// @return Same species
		static bool is_same_species(Genome g1, Genome g2);

		/// @brief Create a basic Genome
		/// @return The new Genome
		static Genome basic_genome();

	private:
	
		/// @brief Randomly shifts/reassigns weights of ALL Genes
		void mutate_point();

		/// @brief Randomly links two neurons together, if they aren't connected
		/// @param force_bias Forces the input to connect to the Bias input
		void mutate_link(bool force_bias);

		/// @brief Splits a random connection in two.
		void mutate_node();
		
		/// @brief Picks a random Gene to set to enable or disable. Only selects Genes that aren't already set to the passed argument.
		/// @param enabled Whether the picked Gene should be enabled
		void mutate_enabled(bool enabled);

	};

	class Species
	{
	public:
		float top_fitness = -FLT_MAX;
		float average_fitness = 0;
		int staleness = 0;
		std::vector<Genome> genomes;

		/// @brief Calculates the average fitness of the Genomes, storing result in `average_fitness`
		void calculate_average_fitness();

		/// @brief Creates a new child within a Species, then mutates it. This will either perform crossover on two Genomes, or duplicate a Genome instead.
		/// @return The new Genome
		Genome breed_child();
	};

	class Ecosystem
	{
	public:
		std::vector<Species> species;
		int generation = 0;
		int innovation = PARAMS.output_count;
		float max_fitness = -FLT_MAX;
		int population = PARAMS.population;
		
		/// @brief The index of the current Genome being evaluated
		int cur_genome = 0;

		/// @brief The current Genome object being evaluated
		Genome* cur_genome_object;

		/// @brief The index of the current Species being evaluated
		int cur_species = 0;

		/// @brief Iterates through all Genomes and ranks them globally, setting their `global_rank` member variables.
		void rank_globally();

		/// @brief Returns the total average fitness of all Species
		/// @return Total average fitness
		float get_total_average_fitness();

		/// @brief Iterates thru each Species and culls unfit Genomes. Reduces population of each Species by 50%, or to 1 member if `cut_to_one` is set.
		/// @param cut_to_one Will cull all Species down to 1 member each
		void cull_species(bool cut_to_one);

		/// @brief Evaluates staleness of each Species, and deletes Species that have too high of a staleness value.
		///
		/// Staleness is increased if the top fitness score in the Species has not increased since the last this was evaluated.
		///
		/// If all species are marked for deletion, then none are deleted. However, the staleness evaluations are preserved.
		void remove_stale_species();

		/// @brief Removes Species that have an average fitness value lower than the total average fitness of the Ecosystem.
		void remove_weak_species();

		/// @brief Inserts a Genome into the Species that is the same species as the Genome.
		///
		/// Or, if there is no Species that the Genome fits into, creates a new Species with the Genome as the sole member.
		/// @param genome Genome
		void add_genome(Genome genome);

		/// @brief Culls unfit Genomes and Species from the population, breeds new children in each Species, culls each Species down to one Genome each, then finally breeds new children until the max population is reached.
		void new_generation();

		/// @brief Calculates the fitness for each Genome in the Ecosystem.
		///
		/// For each Genome, the callback function is called with the Genome as an argument. The callback function must return a fitness value.
		void calculate_fitness(float (*callback)(Genome& genome));

		/// @brief Creates an Ecosystem, sets it as the current global Ecosystem, and populates it with basic Genomes until the population is at maximum.
		/// @return The created Ecosystem
		static Ecosystem init_ecosystem();

	};

	

	


	Ecosystem* CURRENT_ECOSYSTEM;

	/// @brief Generates a new Innovation ID to be used on a Gene
	/// @return Innovation ID
	int new_innovation();
	
	/// @brief Executes sigmoid function on a number
	/// @param x 
	/// @return Result
	float sigmoid(float x);




}
