// Online C++ compiler to run C++ program online
#include <iostream>
#include <vector>
#include <numeric>
#include <variant>
#include <string>
#include <algorithm>
#include <chrono>
#include <string.h>

/*
template<typename ET>
class t8_eclass_schemes : virtual_base_scheme
{
    element_array<ET> mempool;
}

class SCHEME
{
    t8_eclass_schemes[NUM_CLASSES] eclass_schemes;
}*/

/**
 * Templated array of elements
 * 
 * @tparam ET Type of elements
 */
template <typename ET>
class element_array
{
    public:
        element_array (const size_t n){
            elements = std::vector<ET> (n);
        }
        std::vector<ET> elements;
};

class universal_elem_array
{
    public:
        std::variant<element_array<int>, element_array<double>> array;
}

template <>
class element_array <int>
{
    public:
        /**
         * Fill integer element array with increasing numbers starting by 0, multiply by 7
         * so that we can devide later on by 7 without any roundoff errors. 
         * 
         * @param n 
         */
        element_array (const size_t n){
            elements = std::vector<int> (n);
            std::iota(elements.begin(), elements.end(), 0);
            std::for_each(elements.begin(), elements.end(), [](auto &elem) {elem*=7;});
        }

        std::vector<int> elements;
};

template <>
class element_array< double>
{
    public:
        element_array (const size_t n){
            elements = std::vector<double> (n);
            int i = 0;
            for(auto ielem =elements.begin(); ielem != elements.end(); ielem++){
                *ielem = (double)i / (double)(i+1);
                i++;
            }
        }

        std::vector<double> elements;
};

/**
 * A tree to store leave elements and to demonstrate algorithms on trees. 
 * 
 * @tparam ET 
 */
template <typename ET>
class tree
{   
    public:
        void 
        iterate_tree() {
            std::for_each(leaves.elements.begin(), leaves.elements.end(), [](auto& elem) {elem /= 7;});
        }

        void
        print_tree(){
            std::for_each(leaves.elements.begin(), leaves.elements.end(), 
                [](auto& elem) {std::cout<< elem <<std::endl;}
            );
            
        }

        tree (const size_t n) : leaves(n){};

        element_array<ET> leaves;
};

/**
 * A wrapper class to hold every type of tree. 
 * 
 */
class universal_tree
{
    public:
        template<typename T>
        universal_tree(const tree<T>& new_tree) : generic_tree(new_tree) {}

        template<typename T>
        universal_tree(tree<T>&& new_tree) : generic_tree(std::move(new_tree)) {}

        void iterate_tree(){
            std::visit([](auto&& tree){tree.iterate_tree();}, generic_tree);
        }

        void print_tree(){
            std::visit([](auto && tree){tree.print_tree();}, generic_tree);
        }

        std::variant<tree<int>, tree<double>> generic_tree;
};

class forest{
    public:
        forest(const size_t num_trees, const size_t elems_per_tree){
            for(size_t itree = 0; itree < num_trees; itree++){
                if(itree % 2){
                    tree<int> int_tree(elems_per_tree);
                    all_trees.push_back(universal_tree(std::move(int_tree)));
                }
                else{
                    tree<double> double_tree(elems_per_tree);
                    all_trees.push_back(universal_tree(std::move(double_tree)));
                }
            }
        }
        /**
         * Iterate over the tree and do some work on each tree.
         */
        void iterate(){
            for(size_t itree = 0; itree < num_trees(); itree++){
                all_trees[itree].iterate_tree();
            }
        }

        /**
         * Print each tree. 
         */
        void
        print(){
            for(size_t itree = 0; itree < num_trees(); itree++){
                all_trees[itree].print_tree();
            }
        }
        size_t num_trees(){return all_trees.size();}
        std::vector<universal_tree> all_trees;
};

/* ------------ Compare with typecasting ---------------- */

class old_element_array
{
    public:
        old_element_array(const size_t size, const int type_size, int type) :
            type_size(type_size), num_elem(size), type(type){
                array = malloc(num_elem * type_size);
            }

        inline void *
        get_at(size_t index){
            return array + type_size * index;
        }

        old_element_array(const old_element_array &other) : 
        type_size(other.type_size), num_elem(other.num_elem), type(other.type){
            array = malloc(other.num_elem * other.type_size);
            memcpy(array, other.array, other.num_elem * other.type_size);

        }

        ~old_element_array(){
            free(array);
        }

        void *array;
        int type_size;
        size_t num_elem;
        int type;
};

class old_tree{
    public:
        old_tree(const int type, const int type_size, const size_t size) :
        leaves(size, type_size, type), type(type)
        {}

        old_tree(const old_tree &other) :
            leaves(other.leaves), type(other.type)
        {
        }

        inline void *
        get_elem(size_t index){
            return leaves.get_at(index);
        }


        old_element_array leaves;
        int type;
};

class old_forest{
    public:
    old_forest(const size_t num_trees, const size_t elem_per_trees){
        for(size_t itree = 0; itree < num_trees; itree++){
            if(itree % 2){
                old_tree int_tree(0, sizeof(int), elem_per_trees);
                for(size_t ielem = 0; ielem < elem_per_trees; ielem++){
                    int *leaf = (int *)int_tree.get_elem(ielem);
                    *leaf = 7 * ielem;
                }
                all_trees.push_back(int_tree);
            }
            else{
                old_tree double_tree(1, sizeof(double), elem_per_trees);
                for(size_t ielem = 0; ielem < elem_per_trees; ielem++){
                    double *leaf = (double *)double_tree.get_elem(ielem);
                    *leaf = (double)ielem / (double)(ielem+1);
                }
                all_trees.push_back(double_tree);
            }
        }
    }

    size_t num_trees(){return all_trees.size();}    

    void iterate(){
        for(size_t itree = 0; itree < num_trees(); itree++){
            const size_t num_elems = all_trees[itree].leaves.num_elem;
            if(itree%2){
                for(size_t ielem = 0; ielem < num_elems; ielem++){
                    int * leaf = (int *)all_trees[itree].get_elem(ielem);
                    *leaf /= 7;
                }
            }
            else{
                for(size_t ielem = 0; ielem < num_elems; ielem++){
                    double * leaf = (double *) all_trees[itree].get_elem(ielem);
                    *leaf /= 7;
                }
            }
        }
    }

    void print(){
        for(size_t itree = 0; itree < num_trees(); itree++){
            const size_t num_elems = all_trees[itree].leaves.num_elem;
            if(itree%2){
                for(size_t ielem = 0; ielem < num_elems; ielem++){
                    int * leaf = (int *)all_trees[itree].get_elem(ielem);
                    std::cout<<*leaf<<std::endl;
                }
            }
            else{
                for(size_t ielem = 0; ielem < num_elems; ielem++){
                    double * leaf = (double *)all_trees[itree].get_elem(ielem);
                    std::cout<<*leaf<<std::endl;
                }
            }
        }
    }
    std::vector<old_tree> all_trees;
};




int main() {
    size_t num_trees = 2;
    size_t elems_per_tree = 2147483648;

    //std::cout<< "How many trees do you want to use:\n";
    //std::cin>>num_trees;

    //std::cout<< "How many elements per tree:\n";
    //std::cin>>elems_per_tree;

    for(int i = 1; i <= 32; ++i){
        num_trees *= 2;
        elems_per_tree /= 2;
        std::cout<<"--------------------------------------------------------------\n\n";
        std::cout<<"Total number of elements: " << num_trees * elems_per_tree <<std::endl;
        std::cout<<"Num Trees: "<<num_trees <<" Num elems: " <<elems_per_tree <<std::endl;
        auto start_build = std::chrono::high_resolution_clock::now();
        forest my_forest(num_trees, elems_per_tree);
        auto end_build = std::chrono::high_resolution_clock::now();
        auto ms_build = std::chrono::duration_cast<std::chrono::milliseconds>(end_build - start_build);
        std::cout<<"Build time: " << ms_build.count() <<"ms\n";

        auto start_iter = std::chrono::high_resolution_clock::now();
        my_forest.iterate();
        auto end_iter = std::chrono::high_resolution_clock::now();
        auto ms_iter = std::chrono::duration_cast<std::chrono::milliseconds>(end_iter - start_iter);
        std::cout<<"Iter time: " << ms_iter.count() <<"ms\n";

        start_build = std::chrono::high_resolution_clock::now();
        old_forest my_old_forest(num_trees, elems_per_tree);
        end_build = std::chrono::high_resolution_clock::now();
        ms_build = std::chrono::duration_cast<std::chrono::milliseconds>(end_build - start_build);
        std::cout<<"Old build time: " << ms_build.count() <<"ms\n";

        start_iter = std::chrono::high_resolution_clock::now();
        my_old_forest.iterate();
        end_iter = std::chrono::high_resolution_clock::now();
        ms_iter = std::chrono::duration_cast<std::chrono::milliseconds>(end_iter - start_iter);
        std::cout<<"Old iter time: " << ms_iter.count() <<"ms\n\n";

    }
    //old_forest_print(my_old_forest);
    //forest_print(my_forest);
    return 0;
}