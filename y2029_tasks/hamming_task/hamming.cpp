#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>


bool IsPowerOfTwo(int x) {
    return x > 0 && (x & (x - 1)) == 0;
}

// Formula: 2^r >= n + 1
int ComputeParityBitsCount(int n) {
  int r = 0;
  int pow2 = 1;
  while (pow2 < n + 1) {
    pow2 <<= 1;
    ++r;
  }
  return r;
}

// Check validity of Hamming parameters m and n
bool IsValidHammingParams(int m, int n) {
  if (m < 0 || n < 0) return false;
  if (n < m) return false;

  const int r = n - m;
  int pow2 = 1;
  for (int i = 0; i < r; ++i) pow2 <<= 1;
  return pow2 >= n + 1;
}

/* 
 *  Hamming encoding
 *  Returns empty string on error
 */
std::string HammingEncode(int m, int n, const std::string& data) {
  if (!IsValidHammingParams(m, n)) {
    std::cerr << "Error: invalid m and n for Hamming code.\n";
    return std::string();
  }
  if (static_cast<int>(data.size()) != m) {
    std::cerr << "Error: data length does not match m.\n";
    return std::string();
  }
  for (char c : data) {
    if (c != '0' && c != '1') {
      std::cerr << "Error: data must contain only '0' and '1'.\n";
      return std::string();
    }
  }

  std::vector<int> code(n + 1, 0);

  // Fill data bits
  int data_index = 0;
  for (int i = 1; i <= n; ++i) {
    if (!IsPowerOfTwo(i)) {
      code[i] = (data[data_index] == '1') ? 1 : 0;
      ++data_index;
    }
  }

  // Compute parity bits (even parity)
  for (int p = 1; p <= n; p <<= 1) {
    int parity = 0;
    for (int i = 1; i <= n; ++i) {
      if ((i & p) && i != p) {
        parity ^= code[i];
      }
    }
    code[p] = parity;
  }

  std::string result;
  result.reserve(n);
  for (int i = 1; i <= n; ++i) {
    result.push_back(code[i] ? '1' : '0');
  }
  return result;
}

/* 
 *  Hamming decoding with single-bit error correction
 *  out_r — number of parity bits
 *  Returns empty string on error
 */ 
std::string HammingDecode(int n, const std::string& code_str, int* out_r) {
  if (out_r == nullptr) {
    std::cerr << "Error: out_r is null.\n";
    return std::string();
  }
  if (static_cast<int>(code_str.size()) != n) {
    std::cerr << "Error: code length does not match n.\n";
    return std::string();
  }

  for (char c : code_str) {
    if (c != '0' && c != '1') {
      std::cerr << "Error: code must contain only '0' and '1'.\n";
      return std::string();
    }
  }

  *out_r = ComputeParityBitsCount(n);

  std::vector<int> code(n + 1, 0);
  for (int i = 0; i < n; ++i) {
    code[i + 1] = (code_str[i] == '1') ? 1 : 0;
  }

  int error_pos = 0;
  for (int p = 1; p <= n; p <<= 1) {
    int parity = 0;
    for (int i = 1; i <= n; ++i) {
      if (i & p) parity ^= code[i];
    }
    if (parity) error_pos += p;
  }

  if (error_pos >= 1 && error_pos <= n) {
    code[error_pos] ^= 1;
  }

  std::string data;
  data.reserve(n - *out_r);
  for (int i = 1; i <= n; ++i) {
    if (!IsPowerOfTwo(i)) {
      data.push_back(code[i] ? '1' : '0');
    }
  }
  return data;
}

// Check if code word is valid
bool HammingIsValid(int n, const std::string& code_str) {
  if (static_cast<int>(code_str.size()) != n) return false;

  std::vector<int> code(n + 1, 0);
  for (int i = 0; i < n; ++i) {
    const char c = code_str[i];
    if (c != '0' && c != '1') return false;
    code[i + 1] = (c == '1') ? 1 : 0;
  }

  int error_pos = 0;
  for (int p = 1; p <= n; p <<= 1) {
    int parity = 0;
    for (int i = 1; i <= n; ++i) {
      if (i & p) parity ^= code[i];
    }
    if (parity) error_pos += p;
  }

  return error_pos == 0;
}

void PrintUsage(const char* prog) {
  std::cerr << "Usage:\n"
            << "  " << prog << " encode m n bin_vec\n"
            << "  " << prog << " decode n bin_vec\n"
            << "  " << prog << " is_valid n bin_vec\n";
}

bool ParseInt(const char* s, int* out) {
  if (out == nullptr) return false;
  try {
    size_t pos = 0;
    int value = std::stoi(std::string(s), &pos);
    if (s[pos] != '\0') return false;
    *out = value;
    return true;
  } catch (...) {
    return false;
  }
}

#ifndef DEBUG

int main(int argc, char* argv[]) {
  if (argc < 2) {
    PrintUsage(argv[0]);
    return 1;
  }

  const std::string cmd(argv[1]);

  if (cmd == "encode") {
    if (argc != 5) {
      PrintUsage(argv[0]);
      return 1;
    }
    int m = 0;
    int n = 0;
    if (!ParseInt(argv[2], &m) || !ParseInt(argv[3], &n)) {
      std::cerr << "Error: m and n must be integers.\n";
      return 1;
    }
    const std::string bin_vec(argv[4]);

    const std::string encoded = HammingEncode(m, n, bin_vec);
    if (encoded.empty() && n != 0) return 1;
    std::cout << encoded << '\n';
    return 0;
  }

  if (cmd == "decode") {
    if (argc != 4) {
      PrintUsage(argv[0]);
      return 1;
    }
    int n = 0;
    if (!ParseInt(argv[2], &n)) {
      std::cerr << "Error: n must be an integer.\n";
      return 1;
    }
    const std::string bin_vec(argv[3]);

    int r = 0;
    const std::string data = HammingDecode(n, bin_vec, &r);
    if (data.empty() && n != 0) return 1;
    std::cout << r << '\n' << data << '\n';
    return 0;
  }

  if (cmd == "is_valid") {
    if (argc != 4) {
      PrintUsage(argv[0]);
      return 1;
    }
    int n = 0;
    if (!ParseInt(argv[2], &n)) {
      std::cerr << "Error: n must be an integer.\n";
      return 1;
    }
    const std::string bin_vec(argv[3]);

    std::cout << (HammingIsValid(n, bin_vec) ? 1 : 0) << '\n';
    return 0;
  }

  std::cerr << "Unknown command: " << cmd << '\n';
  PrintUsage(argv[0]);
  return 1;
}

#endif