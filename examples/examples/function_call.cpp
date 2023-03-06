#include <iostream>
#include <assert.h>
#include <experimental/mdspan>
#include <span>
#include <ranges>
#include <concepts>

namespace stdex = std::experimental;

template<typename T, typename Extents, typename Layout>
void foo(stdex::mdspan<T,Extents,Layout> A){
	for(int c = 0; c < A.extent(1); c++) {
			stdex::dextents<std::size_t,1> sub_ext(3);
			std::array strides{3};
			stdex::layout_stride::mapping sub_map(sub_ext, strides);
			stdex::mdspan<int, stdex::dextents<size_t,1>,stdex::layout_stride> col_c(&A(0,c), sub_map);

			// foo_1d_mdspan(col_c);
			for (int i = 0; i < col_c.extent(0); i++)
			{
					std::cout << "[" << i << "]: " << col_c(i) << "\n";
			}
		}
}

template<typename T, class Extents>
requires (Extents::rank() == 1)
void print(const stdex::mdspan<T, Extents>& view){
	std::cout <<"\t\t";
	for(int i=0; i < view.extent(0); ++i){
			std::cout << view(i) << ", ";
	}
}

template<typename T, class Extents>
requires (Extents::rank() == 2)
void print(const stdex::mdspan<T, Extents>& view){
	for(int i=0; i < view.extent(0); ++i){
		if(i != 0) std::cout << "\n";
			std::cout <<"\t";
		for(int j = 0; j < view.extent(1); ++j){
			std::cout << view(i,j) << ", ";
		}
	}
}

template<typename Extents = stdex::dextents<size_t, 1>, typename Layout = stdex::layout_right> 
auto make_mdspan(std::ranges::forward_range auto&& view, Extents extents, typename Layout::mapping l){
	std::cout << "Making mdspan...\n";
	using T = std::ranges::range_value_t<decltype(view)>;
	if constexpr(std::is_same_v<stdex::layout_right,Layout> || std::is_same_v<stdex::layout_right,Layout>)
		return stdex::mdspan<T, Extents, Layout>(std::ranges::data(view), extents);
	else
		return stdex::mdspan<T, Extents, Layout>(std::ranges::data(view), extents, l);
}

template<template<typename T, typename...Idx> typename Extents, typename T, typename ...Idx> 
auto make_column_view(std::ranges::forward_range auto&& view, Extents<T,Idx...> extents, size_t col){
	static_assert(Extents<T,Idx...>::rank() == 2);
	assert(col < view.extents(1));
	using range_type = std::ranges::range_value_t<decltype(view)>;

	using extent_type = stdex::extents<typename Extents<T,Idx...>::index_type, stdex::dynamic_extent>;
	extent_type new_extends(extents.extent(1));
	std::array stride{extents.extent(0)};
	stdex::layout_left::mapping column_map(new_extends, stride);
	return make_mdspan(view, new_extends, column_map);

}


template<typename T, typename Extents = stdex::dextents<size_t, 1>, typename Layout = stdex::layout_right>
requires std::is_arithmetic_v<std::remove_cvref_t<T>>
auto make_mdspan(T& value, Extents extents){
	std::cout << "Scalar conversion...\n";
	return make_mdspan(std::span(&value,1), extents);
}


template<typename T, typename Extents = stdex::dextents<size_t, 1>, typename Layout = stdex::layout_right>
void send(const stdex::mdspan<T, Extents, Layout>& view){
	std::cout << "Sending...\n";
	//...do something
	std::cout << typeid(std::decay_t<decltype(view)>).name() << ":\n";
	print(view);
	
	std::cout << "\n";
}

template<typename T, typename Extents>
void send(T&& data, Extents extent){
	std::cout << "-----------------\nDispatching...\n";
	send(make_mdspan(std::forward<T>(data),extent));
}


int main(int argc, char** argv)
{
	if(argc < 2)
		throw std::runtime_error("Size not provided");
	
	size_t size = strtol(argv[1],NULL,10);
	std::cout << "SIze: " << size << "\n";
	int data[] = {
		1, 2, 3,
		4, 5, 6,
		7, 8, 9};
	int *dynamic_data = new int[size];
	std::iota(dynamic_data, dynamic_data + size, 1);
	
	std::vector<int> vector(data, data+9);
	std::array<int,9> array({1,2,3,4,5,6,7,8,9});
	stdex::mdspan<int, stdex::dextents<size_t,2>> A(data, 3, 3);
	int value = 10;

	// foo(A); //pass mdspan
	auto extents = stdex::dextents<size_t,2>(3,3);
	send(vector, extents);
	send(array, extents);
	send(data, extents);
	send(std::span(dynamic_data,size), extents);
	send(value, extents);






	
	free(dynamic_data);
	return 0;
}