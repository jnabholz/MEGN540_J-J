#include "Filter.h"

/**
 * Function Filter_Init initializes the filter given two float arrays and the order of the filter.  Note that the
 * size of the array will be one larger than the order. (First order systems have two coefficients).
 *
 *  1/A_0*( SUM( B_i * input_i )  -   SUM( A_i * output_i) )
 *         i=0..N                    i=1..N
 *
 *  Note a 5-point moving average filter has coefficients:
 *      numerator_coeffs   = { 5 0 0 0 0 };
 *      denominator_coeffs = { 1 1 1 1 1 };
 *      order = 4;
 *
 * @param p_filt pointer to the filter object
 * @param numerator_coeffs The numerator coefficients (B/beta traditionally)
 * @param denominator_coeffs The denominator coefficients (A/alpha traditionally)
 * @param order The filter order
 */
void  Filter_Init ( Filter_Data_t* p_filt, float* numerator_coeffs, float* denominator_coeffs, uint8_t order )
{
    //Initialize the ring buffer in the filter p_filt
    //Need to call those ring buffers correctly
    rb_initialize_F(&(p_filt->numerator));
    rb_initialize_F(&(p_filt->denominator));
    rb_initialize_F(&(p_filt->in_list));
    rb_initialize_F(&(p_filt->out_list));
    
    //Now that the ring buffers are initialized, we have to fill them with the
    //correct values!
    for(int i=0;i<(order+1);i++)
    {
        rb_push_back_F(&(p_filt->numerator),numerator_coeffs[i]);
        rb_push_back_F(&(p_filt->denominator),denominator_coeffs[i]);
        rb_push_front_F(&(p_filt->in_list),0);
        rb_push_back_F(&(p_filt->out_list),0);
    }
	return;
}

/**
 * Function Filter_ShiftBy shifts the input list and output list to keep the filter in the same frame. This especially
 * useful when initializing the filter to the current value or handling wrapping/overflow issues.
 * @param p_filt
 * @param shift_amount
 */
void  Filter_ShiftBy( Filter_Data_t* p_filt, float shift_amount )
{
    //Increment each value in the in_list and out_list ring buffers by shift_amount
    for(int i=0;i<rb_length_F(&(p_filt -> in_list));i++)
    {
        rb_set_F(&(p_filt -> in_list),i,rb_get_F(&(p_filt -> in_list),i)+shift_amount);
        rb_set_F(&(p_filt -> out_list),i,rb_get_F(&(p_filt -> out_list),i)+shift_amount);
    }
    return;
}

/**
 * Function Filter_SetTo sets the initial values for the input and output lists to a constant defined value. This
 * helps to initialize or re-initialize the filter as desired.
 * @param p_filt Pointer to a Filter_Data sturcture
 * @param amount The value to re-initialize the filter to.
 */
void Filter_SetTo( Filter_Data_t* p_filt, float amount )
{
    //Create a for loop and set each initial value
    for(int i=0;i<rb_length_F(&(p_filt -> in_list));i++)
    {
        rb_set_F(&(p_filt -> in_list),i,amount);
        rb_set_F(&(p_filt -> out_list),i,amount);
    }
    return;
}

/**
 * Function Filter_Value adds a new value to the filter and returns the new output.
 * @param p_filt pointer to the filter object
 * @param value the new measurement or value
 * @return The newly filtered value
 */
float Filter_Value( Filter_Data_t* p_filt, float value)
{
    //Store the new input value in the input list
    rb_push_front_F(&(p_filt -> in_list),value);
    //Remove the rearmost value
    rb_pop_back_F(&(p_filt -> in_list));
    //Initialize a new float new_out
    float new_out=0;
    //Store the ring buffer length as an integer so we only call it once
    int length=rb_length_F(&(p_filt -> in_list));
    /*Sum the products of B_i and the corresponding input values and add them
    to new_out*/
    for(int i=0;i<length;i++)
    {
        new_out=new_out+rb_get_F(&(p_filt -> numerator),i)*rb_get_F(&(p_filt -> in_list),i);
    }
    /*Sum the products of A_i and the corresponding output values and subtract
    them from new_out*/
    for(int i=1;i<length;i++)
    {
        new_out=new_out-rb_get_F(&(p_filt -> denominator),i)*rb_get_F(&(p_filt -> out_list),i-1);
    }
    //Divide new_out by A_0 to get the filtered value
    new_out=new_out/rb_get_F(&(p_filt -> denominator),0);
    /*Store new_out in the output list, pop out the old output, and return the
    new output*/
    rb_push_front_F(&(p_filt -> out_list),new_out);
    rb_pop_back_F(&(p_filt -> out_list));
	return new_out;
}

/**
 * Function Filter_Last_Output returns the most up-to-date filtered value without updating the filter.
 * @return The latest filtered value
 */
float Filter_Last_Output( Filter_Data_t* p_filt )
{
    //Pop out the frontmost value in out_list, push it back in, and return it!
    float value=rb_pop_front_F(&(p_filt->out_list));
    rb_push_front_F(&(p_filt -> out_list),value);
	return value;
}
