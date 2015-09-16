extern void test_cx_counter();
extern void test_cx_guid();
extern void test_cx_hash();
extern void test_cx_math();
extern void test_cx_pcg32();

int main(int, char* [])
{
  test_cx_counter();
  test_cx_guid();
  test_cx_hash();
  test_cx_math();
  test_cx_pcg32();

  return 0;
}
