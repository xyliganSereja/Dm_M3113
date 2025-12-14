package main

import (
	"fmt"
	"log/slog"
	"os"
	"strconv"
)

func encode(m, n int, data []int) ([]int, bool) {
	r := countR(n)
	if n != m+r {
		return nil, false
	}
	if len(data) != m {
		return nil, false
	}

	code := make([]int, n)

	di := 0
	for pos := 1; pos <= n; pos++ {
		if isPowerOfTwo(pos) {
			code[pos-1] = 0
		} else {
			code[pos-1] = data[di]
			di++
		}
	}

	for i := 0; i < r; i++ {
		p := 1 << i
		parity := 0
		for pos := 1; pos <= n; pos++ {
			if (pos & p) != 0 {
				parity ^= code[pos-1] // XOR
			}
		}
		code[p-1] = parity
	}

	return code, true
}

func error_position(code []int) int {
	n := len(code)
	r := countR(n)
	s := 0
	for i := 0; i < r; i++ {
		p := 1 << i
		parity := 0
		for pos := 1; pos <= n; pos++ {
			if (pos & p) != 0 {
				parity ^= code[pos-1]
			}
		}
		if parity != 0 {
			s |= p
		}
	}
	return s
}

func decode(n int, code []int) (int, []int, bool) {
	if len(code) != n {
		return 0, nil, false
	}
	r := countR(n)

	s := error_position(code)
	if s != 0 && s <= n {
		code[s-1] ^= 1
	}

	data := make([]int, 0, n-r)
	for pos := 1; pos <= n; pos++ {
		if !isPowerOfTwo(pos) {
			data = append(data, code[pos-1])
		}
	}
	return r, data, true
}

func isValid(n int, code []int) bool {
	if len(code) != n {
		return false
	}
	return error_position(code) == 0
}

func main() {
	if len(os.Args) < 2 {
		return
	}

	comand := os.Args[1]

	switch comand {
	case "encode":
		if len(os.Args) != 5 {
			slog.Error("Incorrect arg count")
			return
		}
		m, err := strconv.Atoi(os.Args[2])
		if err != nil {
			slog.Error("Cant parse m")
			return
		}
		n, err := strconv.Atoi(os.Args[3])
		if err != nil {
			slog.Error("Cant parse n")
			return
		}
		data, ok := parseStringData(os.Args[4], m)
		if !ok {
			slog.Error("Cant parse data")
			return
		}
		code, ok := encode(m, n, data)
		if !ok {
			slog.Error("Failed encoding process")
			return
		}
		fmt.Println(intArrToString(code))

	case "decode":
		if len(os.Args) != 4 {
			slog.Error("Incorrect arg count")
			return
		}
		n, err := strconv.Atoi(os.Args[2])
		if err != nil {
			slog.Error("Cant parse n")
			return
		}
		code, ok := parseStringData(os.Args[3], n)
		if !ok {
			slog.Error("Cant parse data")
			return
		}
		r, data, ok := decode(n, code)
		if !ok {
			slog.Error("Failed decoding process")
			return
		}
		fmt.Println(r)
		fmt.Println(intArrToString(data))

	case "is_valid":
		if len(os.Args) != 4 {
			slog.Error("Incorrect arg count")
			return
		}
		n, err := strconv.Atoi(os.Args[2])
		if err != nil {
			slog.Error("Cant parse n")
			return
		}
		code, ok := parseStringData(os.Args[3], n)
		if !ok {
			slog.Error("Cant parse data")
			return
		}
		if isValid(n, code) {
			fmt.Println(1)
		} else {
			fmt.Println(0)
		}

	default:
		slog.Error("Undefined command")
	}
}
