#define DEBUG
#include "hamming.cpp"

#include <iostream>
#include <string>
#include <vector>


struct Stats {
  int passed = 0;
  int failed = 0;
  int test_no = 0;
};

static void BeginTest(Stats& st, const std::string& name) {
  ++st.test_no;
  std::cout << "#" << (st.test_no < 10 ? "0" : "") << st.test_no << ": " << name << "\n";
}

static void Pass(Stats& st) {
  ++st.passed;
  std::cout << "  [OK]\n\n";
}

static void Fail(Stats& st, const std::string& msg) {
  ++st.failed;
  std::cout << "  [FAIL] " << msg << "\n\n";
}

static bool ExpectEqStr(Stats& st, const std::string& what, const std::string& got, const std::string& expected) {
  if (got == expected) return true;
  Fail(st, what + "\n       expected: \"" + expected + "\"\n       got:      \"" + got + "\"");
  return false;
}

static bool ExpectEqInt(Stats& st, const std::string& what, int got, int expected) {
  if (got == expected) return true;
  Fail(st, what + "\n       expected: " + std::to_string(expected) + "\n       got:      " + std::to_string(got));
  return false;
}

static bool ExpectTrue(Stats& st, const std::string& what, bool cond) {
  if (cond) return true;
  Fail(st, what + "\n       expected: true\n       got:      false");
  return false;
}


static std::string FlipBit(const std::string& s, int pos) {
  std::string r = s;
  if (pos >= 0 && pos < (int)r.size()) {
    r[pos] = (r[pos] == '0') ? '1' : '0';
  }
  return r;
}

static std::string ToBinary(int x, int bits) {
  std::string s(bits, '0');
  for (int i = bits - 1; i >= 0; --i) {
    s[i] = (x & 1) ? '1' : '0';
    x >>= 1;
  }
  return s;
}

static std::string PatternBits(int len, const std::string& pat) {
  std::string s;
  s.reserve(len);
  while ((int)s.size() < len) s += pat;
  s.resize(len);
  return s;
}

static unsigned NextRand(unsigned& state) {
  state = state * 1664525u + 1013904223u;
  return state;
}

static std::string RandomBits(int len, unsigned seed) {
  std::string s(len, '0');
  unsigned st = seed;
  for (int i = 0; i < len; ++i) {
    s[i] = (NextRand(st) & 1u) ? '1' : '0';
  }
  return s;
}


static void Test_ComputeParityBitsCount(Stats& st) {
  BeginTest(st, "ComputeParityBitsCount basic values");

  if (!ExpectEqInt(st, "r for n=7", ComputeParityBitsCount(7), 3)) return;
  if (!ExpectEqInt(st, "r for n=15", ComputeParityBitsCount(15), 4)) return;
  if (!ExpectEqInt(st, "r for n=31", ComputeParityBitsCount(31), 5)) return;
  if (!ExpectEqInt(st, "r for n=1023", ComputeParityBitsCount(1023), 10)) return;

  Pass(st);
}

static void Test_RoundTrip_7_4_Fixed(Stats& st) {
  BeginTest(st, "roundtrip (7,4) fixed cases");

  const std::vector<std::string> inputs = {"0000", "0010", "1011", "1101", "1111"};
  for (const auto& data : inputs) {
    std::string code = HammingEncode(4, 7, data);

    int r = 0;
    std::string out = HammingDecode(7, code, &r);

    if (!ExpectEqInt(st, "decode r for (7,4)", r, 3)) return;
    if (!ExpectEqStr(st, "roundtrip data for (7,4)", out, data)) return;
  }

  Pass(st);
}

static void Test_Exhaustive_RoundTrip_7_4(Stats& st) {
  BeginTest(st, "exhaustive roundtrip (7,4): all 16 words");

  for (int x = 0; x < 16; ++x) {
    std::string data = ToBinary(x, 4);
    std::string code = HammingEncode(4, 7, data);

    int r = 0;
    std::string out = HammingDecode(7, code, &r);

    if (!ExpectEqStr(st, "data mismatch for data=" + data, out, data)) return;
    if (!ExpectEqInt(st, "r mismatch for data=" + data, r, 3)) return;
  }

  Pass(st);
}

static void Test_Exhaustive_SingleBitCorrection_7_4(Stats& st) {
  BeginTest(st, "single-bit correction (7,4): all 16 words * all 7 positions");

  for (int x = 0; x < 16; ++x) {
    std::string data = ToBinary(x, 4);
    std::string code = HammingEncode(4, 7, data);

    for (int pos = 0; pos < 7; ++pos) {
      std::string corrupted = FlipBit(code, pos);
      int r = 0;
      std::string out = HammingDecode(7, corrupted, &r);

      if (!ExpectEqStr(st, "wrong correction data=" + data + " pos=" + std::to_string(pos), out, data)) return;
      if (!ExpectEqInt(st, "wrong r after correction pos=" + std::to_string(pos), r, 3)) return;
    }
  }

  Pass(st);
}

static void Test_IsValid_Correct_7_4(Stats& st) {
  BeginTest(st, "is_valid (7,4): correct codes => 1");

  for (int x = 0; x < 16; ++x) {
    std::string data = ToBinary(x, 4);
    std::string code = HammingEncode(4, 7, data);

    if (!ExpectTrue(st, "valid code rejected, data=" + data, HammingIsValid(7, code))) return;
  }

  Pass(st);
}

static void Test_RoundTrip_15_11_Fixed(Stats& st) {
  BeginTest(st, "roundtrip (15,11) fixed cases");

  const int m = 11, n = 15;
  const int r_expected = ComputeParityBitsCount(n);

  const std::vector<std::string> inputs = {
    std::string(m, '0'),
    std::string(m, '1'),
    PatternBits(m, "01"),
    PatternBits(m, "10"),
    "10101010101"
  };

  for (const auto& data : inputs) {
    std::string code = HammingEncode(m, n, data);

    int r = 0;
    std::string out = HammingDecode(n, code, &r);

    if (!ExpectEqStr(st, "roundtrip mismatch", out, data)) return;
    if (!ExpectEqInt(st, "r mismatch", r, r_expected)) return;
  }

  Pass(st);
}

static void Test_SingleBitCorrection_15_11_Selected(Stats& st) {
  BeginTest(st, "single-bit correction (15,11): selected cases * all 15 positions");

  const int m = 11, n = 15;
  const int r_expected = ComputeParityBitsCount(n);

  const std::vector<std::string> inputs = {
    std::string(m, '0'),
    std::string(m, '1'),
    PatternBits(m, "01"),
    PatternBits(m, "001"),
    "10101010101"
  };

  for (const auto& data : inputs) {
    std::string code = HammingEncode(m, n, data);

    for (int pos = 0; pos < n; ++pos) {
      std::string corrupted = FlipBit(code, pos);

      int r = 0;
      std::string out = HammingDecode(n, corrupted, &r);

      if (!ExpectEqStr(st, "wrong correction pos=" + std::to_string(pos), out, data)) return;
      if (!ExpectEqInt(st, "wrong r pos=" + std::to_string(pos), r, r_expected)) return;
    }
  }

  Pass(st);
}

static void Test_IsValid_Correct_15_11(Stats& st) {
  BeginTest(st, "is_valid (15,11): correct codes => 1");

  const int m = 11, n = 15;

  const std::vector<std::string> inputs = {
    "10101010101",
    PatternBits(m, "01"),
    PatternBits(m, "111000"),
    std::string(m, '0'),
    std::string(m, '1')
  };

  for (const auto& data : inputs) {
    std::string code = HammingEncode(m, n, data);
    if (!ExpectTrue(st, "valid code rejected", HammingIsValid(n, code))) return;
  }

  Pass(st);
}

static void Test_Random_RoundTrips_31_26(Stats& st) {
  BeginTest(st, "random roundtrips (31,26) x500");

  const int n = 31;
  const int r_expected = ComputeParityBitsCount(n);
  const int m = n - r_expected;

  for (int i = 0; i < 500; ++i) {
    std::string data = RandomBits(m, 12345u + (unsigned)i * 777u);
    std::string code = HammingEncode(m, n, data);

    int r = 0;
    std::string out = HammingDecode(n, code, &r);

    if (!ExpectEqStr(st, "random roundtrip mismatch", out, data)) return;
    if (!ExpectEqInt(st, "wrong r", r, r_expected)) return;
  }

  Pass(st);
}

static void Test_Random_SingleBitCorrection_31_26(Stats& st) {
  BeginTest(st, "random single-bit correction (31,26): 200 messages, 10 flips each");

  const int n = 31;
  const int r_expected = ComputeParityBitsCount(n);
  const int m = n - r_expected;

  for (int i = 0; i < 200; ++i) {
    std::string data = RandomBits(m, 777u + (unsigned)i * 1009u);
    std::string code = HammingEncode(m, n, data);

    unsigned seed = 42u + (unsigned)i * 17u;
    for (int k = 0; k < 10; ++k) {
      int pos = (int)(NextRand(seed) % (unsigned)n);
      std::string corrupted = FlipBit(code, pos);

      int r = 0;
      std::string out = HammingDecode(n, corrupted, &r);

      if (!ExpectEqStr(st, "wrong correction pos=" + std::to_string(pos), out, data)) return;
      if (!ExpectEqInt(st, "wrong r", r, r_expected)) return;
    }
  }

  Pass(st);
}

static void Test_Huge_1023_1013(Stats& st) {
  BeginTest(st, "HUGE (1023,1013) roundtrip + many flips");

  const int n = 1023;
  const int r_expected = ComputeParityBitsCount(n);
  const int m = n - r_expected;

  std::string data = PatternBits(m, "00101101");
  std::string code = HammingEncode(m, n, data);

  int r = 0;
  std::string out = HammingDecode(n, code, &r);

  if (!ExpectEqInt(st, "r mismatch", r, r_expected)) return;
  if (!ExpectEqStr(st, "roundtrip mismatch", out, data)) return;

  std::vector<int> flips = {
    0, 1, 2, 3,
    n / 8, n / 4, 3 * n / 8, n / 2,
    5 * n / 8, 3 * n / 4, 7 * n / 8,
    n - 4, n - 3, n - 2, n - 1
  };

  for (int pos : flips) {
    int rr = 0;
    std::string oo = HammingDecode(n, FlipBit(code, pos), &rr);
    if (!ExpectEqStr(st, "wrong correction pos=" + std::to_string(pos), oo, data)) return;
    if (!ExpectEqInt(st, "wrong r", rr, r_expected)) return;
  }

  Pass(st);
}


int main() {
  Stats st;
  std::cout << "=== Hamming unit tests ===\n\n";

  Test_ComputeParityBitsCount(st);

  Test_RoundTrip_7_4_Fixed(st);
  Test_Exhaustive_RoundTrip_7_4(st);
  Test_Exhaustive_SingleBitCorrection_7_4(st);
  Test_IsValid_Correct_7_4(st);

  Test_RoundTrip_15_11_Fixed(st);
  Test_SingleBitCorrection_15_11_Selected(st);
  Test_IsValid_Correct_15_11(st);

  Test_Random_RoundTrips_31_26(st);
  Test_Random_SingleBitCorrection_31_26(st);

  Test_Huge_1023_1013(st);

  std::cout << "-------------------------\n";
  std::cout << "PASSED: " << st.passed << "\n";
  std::cout << "FAILED: " << st.failed << "\n";
  return (st.failed == 0) ? 0 : 1;
}
