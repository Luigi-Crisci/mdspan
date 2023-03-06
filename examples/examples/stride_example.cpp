#include <iostream>
#include <assert.h>
#include <experimental/mdspan>

namespace stdex = std::experimental;
/*
struct column_layout
{
	template <typename extents>
	struct mapping
	{

		using extent_type = extents;
		using rank_type = typename extents::rank_type;
		using size_type = typename extents::size_type;
		using layout_type = column_layout;

		mapping() noexcept = default;
		mapping(const mapping &) = default;
		mapping &operator=(const mapping &) noexcept = default;

		mapping(const extents& ex, size_t col) : ex(ex), col(col)
		{
			assert(ex.extents(0));
			assert(ex.extents(1)),
			assert(this.col > 0);
		}

		constexpr const extent_type &extents() const
		{
			return ex;
		}

		constexpr size_type
		required_span_size() const noexcept
		{
			return ex.extents(1);
		}

		template <class Index>
		// requires(is_convertible_v<RowIndex, size_type> &&
		//   is_convertible_v<ColIndex, size_type> &&
		//   is_nothrow_constructible_v<size_type, RowIndex> &&
		//   is_nothrow_constructible_v<size_type, ColIndex>)
		constexpr size_type
		operator()(Index idx) const noexcept
		{
			// TODO (mfh 2022/08/04 check precondition that
			// extents_type::index-cast(row, col)
			// is a multidimensional index in extents_.
			return ex.extents(0) * idx + col;
		}

		// Mapping is always unique
		static constexpr bool is_always_unique() noexcept { return true; }
		// Only exhaustive if extents_.extent(0) % column_tile_size == 0, so not always
		static constexpr bool is_always_exhaustive() noexcept { return false; }
		// There is not always a regular stride between elements in a given dimension
		static constexpr bool is_always_strided() noexcept { return false; }

		static constexpr bool is_unique() noexcept { return true; }
		constexpr bool is_exhaustive() const noexcept
		{
			// Only exhaustive if extents fit exactly into tile sizes...
			// return (extents_.extent(0) % row_tile_size_ == 0) && (extents_.extent(1) % col_tile_size_ == 0);
			return true;
		}
		// There are some circumstances where this is strided, but we're not
		// concerned about that optimization, so we're allowed to just return false here
		constexpr bool is_strided() const noexcept { return false; }

	private:
		size_t col;
		extents ex;
	};
};
*/
int main()
{
	int data[] = {
		1, 2, 3,
		4, 5, 6,
		7, 8, 9};
	using extent_type = stdex::dextents<size_t, 1>;
	// stdex::layout_right::mapping mapping(3);

	
	stdex::mdspan<int, stdex::dextents<size_t,2>> A(data, 3, 3);
	
	for(int c = 0; c < A.extent(1); c++) {
		stdex::dextents<std::size_t,1> sub_ext(3);
		std::array strides{3};
		stdex::layout_stride::mapping sub_map(sub_ext, strides);
		stdex::mdspan<int, stdex::dextents<size_t,1>,stdex::layout_stride> col_c(&A(0,c), sub_map);
		std::cout << col_c.extent(0) << "\n";
		std::cout << col_c.extent(1) << "\n";

		// foo_1d_mdspan(col_c);
		for (int i = 0; i < col_c.extent(0); i++)
		{
				std::cout << "[" << i << "]: " << col_c(i) << "\n";
		}
	}

	// for (int i = 0; i < wrap.extent(1); i++)
	// {
	// 		std::cout << "[" << i << wrap(i) << "\n";
	// }

	return 0;
}