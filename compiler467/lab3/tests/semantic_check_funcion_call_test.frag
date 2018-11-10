{
    ivec2 ivec_two;
    bvec2 bvec_two;
    bvec3 bvec_three;
    bvec4 bvec_four;
    ivec3 ivec_three;
    ivec4 ivec_four;
    vec3 vec_three;
    vec4 vec_four;
    float valid_call_rsq = rsq(1.23);
    float valid_call_dp3 = dp3(vec_three, vec_three);
    vec4 valid_call_lit = lit(vec_four);
    float dont_want_vectors = rsq(ivec_four);
    float too_many_arguments = rsq(123, 456);
    float too_few_vectors = dp3(vec_four);
    vec4 unwanted_vec_size = lit(vec_three);
    vec4 unwanted_vec_type = lit(bvec_four);
    float unwanted_vec_type_size = lit(bvec_two);
    float inconsistent_two_vec_sizes = dp3(vec_three, vec_four);
    float inconsistent_two_vec_types = dp3(vec_three, ivec_three);
    float inconsistent_two_vec_size_types = dp3(vec_three, ivec_four);


}