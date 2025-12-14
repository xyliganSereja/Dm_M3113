package main

func isPowerOfTwo(x int) bool {
	if x < 1 {
		return false
	}
	for x%2 == 0 {
		x /= 2
	}
	return x == 1
}

func countR(n int) int {
	r := 0
	power := 1

	for power < n+1 {
		power = power * 2
		r++
	}

	return r
}

func parseStringData(s string, expected int) ([]int, bool) {
	if expected >= 0 && len(s) != expected {
		return nil, false
	}
	v := make([]int, len(s))
	for i, ch := range s {
		if ch == '0' {
			v[i] = 0
		} else if ch == '1' {
			v[i] = 1
		} else {
			return nil, false
		}
	}
	return v, true
}

func intArrToString(v []int) string {
	b := make([]byte, len(v))
	for i := range v {
		if v[i] == 0 {
			b[i] = '0'
		} else {
			b[i] = '1'
		}
	}
	return string(b)
}
