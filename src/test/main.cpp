extern void test_cx_algorithm();
extern void test_cx_array();
extern void test_cx_counter();
extern void test_cx_guid();
extern void test_cx_hash();
extern void test_cx_math();
extern void test_cx_numeric();
extern void test_cx_pcg32();
extern void test_cx_strenc();
extern void test_cx_typeid();
extern void test_cx_utils();

int main(int, char* [])
{
  test_cx_algorithm();
  test_cx_array();
  test_cx_counter();
  test_cx_guid();
  test_cx_hash();
  test_cx_math();
  test_cx_numeric();
  test_cx_pcg32();
  test_cx_strenc();
  test_cx_typeid();
  test_cx_utils();

  return 0;
}
