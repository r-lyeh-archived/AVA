// todo:
// support:
// - 1) Single object 
// - 2) List object[] (N) (methods: add, add_unique, remove_at(idx), remove(elem), remove_all(elem), size, sort, remove_back, remove_front, shrink)
// - 3) Dict map<any,object> (sort: yes/no)
//
// - can be generalized? maybe:
// 3) map<any,object> -> map<hash(key),array_index> + array[values] (**) (fast iteration)
// 2) detach array[values] from map, and forget about indexes
// 1) array of single element

