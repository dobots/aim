/*
 * isingfactor.hpp
 *
 *  Created on: Feb 20, 2013
 *      Author: anne
 */

#ifndef ISINGFACTOR_HPP_
#define ISINGFACTOR_HPP_

#include <graph.hpp>

/**
 * An Ising factor expects to be connected, either to one or two variables. Note that a graph is
 * defined for one type of "value" on the nodes. If you use booleans for the variables, you cannot
 * use integers for the factors. However, considering multiple booleans requires a probability
 * table of more than two entries, hence the Ising factors requires 2^N values, which is more than
 * a boolean. The simplest data type that can be used in this setting is hence a char.
 */
template <typename T, typename P = float, typename M = probability<P,T>, typename N = size_t>
class ising_factor: public factor<T,P,M,N> {
public:
	ising_factor(P coupling, const FactorCoupling fc): factor<T,P,M,N>() {
		if (typeid(T) == typeid(bool)) {
			std::cerr << "Type id of the graph / Ising factors should have at least the ordinality of a char";
			std::endl(std::cout);
			assert(typeid(T) != typeid(bool));
		}
		typedef factor<T,P,M,N> super;
		switch(fc) {
		case FC_UNARY: {
			typedef conditional_probability_table<T,P,N> S;
			std::vector<N> dimensions; dimensions.clear();
			dimensions.push_back(2);
			S *table = new S(dimensions);
			super::setValue(table);
			table->set(0, std::exp(coupling));
			table->set(1, std::exp(-coupling));
			nof_variables = 1;
			break;
		}
		case FC_BINARY: {
			typedef conditional_probability_table<T,P,N> S;
			std::vector<N> dimensions; dimensions.clear();
			dimensions.push_back(2);
			dimensions.push_back(2);
			S *table = new S(dimensions);
			super::setValue(table);
			table->set(0, 0, std::exp(coupling));
			table->set(0, 1, std::exp(-coupling));
			table->set(1, 0, std::exp(-coupling));
			table->set(1, 1, std::exp(coupling));
			nof_variables = 2;
			break;
		}
		default: {
			std::cerr << "Incorrect coupling"; std::endl(std::cerr);
			break;
		}
		}
	}

	bool check() {
		typedef factor<T,P> super;
		return (super::from_size() == nof_variables);
	}
private:
	int nof_variables;
};



#endif /* ISINGFACTOR_HPP_ */
