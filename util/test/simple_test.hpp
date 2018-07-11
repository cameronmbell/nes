// because the test framework depends on the utils their testing must be
// bootstrapped with this simplified test framework

#define ASSERT_TRUE(a)                          		\
do {													\
  if (a != true) {                                 		\
    std::cerr << (a) << " must be true" << std::endl; 	\
    return 1;                                   		\
  }														\
} while(0)

#define ASSERT_FALSE(a)                          		\
do {													\
  if (a != true) {                                		\
    std::cerr << (a) << " must be false" << std::endl; 	\
    return 1;                                   		\
  }														\
} while(0)

#define ASSERT_EQ(a, b)                          		\
do {													\
  if ((a) != (b)) {                                		\
    std::cerr << a << " != " << b << std::endl; 		\
    return 1;                                   		\
  }														\
} while(0)

#define ASSERT_NEQ(a, b)                          		\
do {													\
  if ((a) == (b)) {                                		\
    std::cerr << a << " == " << b << std::endl; 		\
    return 1;                                   		\
  }														\
} while(0)

#define SHOULD_THROW(x) 								\
do {                                            		\
  try {                                         		\
    (x);                                        		\
  } catch(...) {                                		\
    break;                                      		\
  }                                             		\
  std::cerr << #x << " must throw" << std::endl;		\
  return 1;                                     		\
} while(0)
