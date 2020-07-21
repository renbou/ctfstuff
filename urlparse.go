package urlparse

import (
	"encoding/hex"
	"strings"
)

func Unquote(str string) (string) {
	parts := strings.Split(str, "%")
	if len(parts) <= 1 {
		return str
	}
	res := []string{parts[0]}
	for _, item := range parts[1:] {
		var (
			decoded []byte
			err error
		)
		if len(item) >= 2 {
			decoded, err = hex.DecodeString(item[:2])
		}
		if err != nil || len(item) < 2 {
			res = append(res, "%", item)
		} else {
			res = append(res, string(decoded), item[2:])
		}
	}
	return strings.Join(res, "")
}

func UnquotePlus(str string) (string) {
	parts := strings.Split(str, "%")
	if len(parts) <= 1 {
		return str
	}
	res := []string{parts[0]}
	for _, item := range parts[1:] {
		var (
			decoded []byte
			err error
		)
		if len(item) >= 2 {
			decoded, err = hex.DecodeString(item[:2])
		}
		if err != nil || len(item) < 2 {
			res = append(res, "%", item)
		} else {
			res = append(res, string(decoded), item[2:])
		}
	}
	return strings.ReplaceAll(strings.Join(res, ""), "+", " ")
}