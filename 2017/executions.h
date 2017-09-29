void apply_none (state_t result[], int* result_top) {}
void apply_assign (state_t result[], int* result_top)
{
	result[*result_top - 2].number = result[*result_top - 1].number;
	*result_top -= 2;
}
void apply_sum (state_t result[], int* result_top)
{
	result[*result_top - 2].number += result[*result_top].number;
	*result_top -= 2;
}
void apply_mul (state_t result[], int* result_top)
{
	result[*result_top - 2].number *= result[*result_top].number;
	*result_top -= 2;
}

void init_apply()
{
	apply_table[0] = &apply_none;
	apply_table[1] = &apply_sum;
	apply_table[2] = &apply_none;
	apply_table[3] = &apply_mul;
	apply_table[4] = &apply_none;
	apply_table[5] = &apply_assign;
	apply_table[6] = &apply_none;
};
