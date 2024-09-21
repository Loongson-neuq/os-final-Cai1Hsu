void hack_func(long flag, long target_value, long max_search_bytes)
{
    long* p_flag = &flag;
    long* p_target_value = &target_value;

    long* initial_ptr;

    if (p_flag > p_target_value)
        initial_ptr = p_flag - 1;
    else
        initial_ptr = p_target_value;

    for (long* ptr = initial_ptr; ptr < initial_ptr + max_search_bytes; ptr++)
    {
        if (*ptr == flag)
        {
            *ptr = target_value;
        }
    }
}
