#ifndef __IPC_FUNC_TRAITS_H__
#define __IPC_FUNC_TRAITS_H__

namespace ipc 
{

	namespace tags 
	{
		// tags for the function traits, used for tag dispatching
		struct zero_arg {};
		struct nonzero_arg {};
		struct void_result {};
		struct nonvoid_result {};

		template <int N> struct arg_count_trait { typedef nonzero_arg type; };

		template <> struct arg_count_trait<0> { typedef zero_arg type; };

		template <typename T> struct result_trait { typedef nonvoid_result type; };

		template <> struct result_trait<void> { typedef void_result type; };

        struct in_arg {};

        struct out_arg {};
	} // namespace tags

	//! \brief Provides a small function traits implementation that
	//! works with a reasonably large set of functors.
	template <typename T>
	struct func_traits : func_traits<decltype(&T::operator())> {};

	template <typename C, typename R, typename... Args>
	struct func_traits<R(C::*)(Args...)> : func_traits<R(*)(Args...)> {};

	template <typename C, typename R, typename... Args>
	struct func_traits<R(C::*)(Args...) const> : func_traits<R(*)(Args...)> {};

	template <typename R, typename... Args> struct func_traits<R(*)(Args...)> 
	{
		using result_type = R;
		using arg_count = std::integral_constant<std::size_t, sizeof...(Args)>;
		using args_type = std::tuple<typename std::decay<Args>::type...>;
        using args_oringinal_type = std::tuple<Args...>;
	};

	template <typename T>
	struct func_kind_info : func_kind_info<decltype(&T::operator())> {};

	template <typename C, typename R, typename... Args>
	struct func_kind_info<R(C::*)(Args...)> : func_kind_info<R(*)(Args...)> {};

	template <typename C, typename R, typename... Args>
	struct func_kind_info<R(C::*)(Args...) const>
		: func_kind_info<R(*)(Args...)> {};

	template <typename R, typename... Args> struct func_kind_info<R(*)(Args...)> 
	{
		typedef typename tags::arg_count_trait<sizeof...(Args)>::type args_kind;
		typedef typename tags::result_trait<R>::type result_kind;
	};

	template <typename F>
	using is_void_result = std::is_void<typename func_traits<F>::result_type>;

    template <typename T> 
    struct type_redefine
    {
        using type = T&;
    };

    template <typename T> 
    struct type_redefine<const T>
    {
        using type = T;
    };
	
	template <typename... ARGS>
    struct args_check
    {
        template <typename T>
        struct arg_in
        {
            // TODO:: Currently, checking if the parameter is const to decide it is in or out.
            constexpr static const bool value = std::is_const<typename std::remove_reference<T>::type>::value;// || !std::is_lvalue_reference<T>::value;
        };

        template <typename T>
        struct arg_out
        {
            constexpr static const bool value = !arg_in<T>::value;
        };

        template <typename ARG_TAG, typename FIRST, typename... REST>
        constexpr inline static bool check(
            std::enable_if_t<(std::is_same<ARG_TAG, tags::in_arg>::value && arg_in<FIRST>::value) 
                || (std::is_same<ARG_TAG, tags::out_arg>::value && arg_out<FIRST>::value)>* = nullptr)
        {
            return true;
        }

        template <typename ARG_TAG, typename FIRST, typename... REST>
        constexpr inline static bool check(
            std::enable_if_t<(std::is_same<ARG_TAG, tags::in_arg>::value && arg_out<FIRST>::value) 
                || (std::is_same<ARG_TAG, tags::out_arg>::value && arg_in<FIRST>::value)>* = nullptr)
        {
            return check<ARG_TAG, REST...>();
        }

        template <typename ARG_TAG>
        constexpr inline static bool check()
        {
            return false;
        }

        constexpr static const bool has_in_  = check<tags::in_arg, ARGS...>();
        constexpr static const bool has_out_ = check<tags::out_arg, ARGS...>();
    };

    template <typename... ARGS>
    struct args_check<std::tuple<ARGS...>> : args_check<ARGS...> {};
} // namespace detail

#endif // __IPC_FUNC_TRAITS_H__

