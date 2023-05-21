package formater

import (
	"bufio"
	"log"
	"os"
	"strings"
)

/*
type mpuValues struct {
	AccelX int
	AccelY int
	AccelZ int
	GyroX  int
	GyroY  int
	GyroZ  int
}
*/

func FormatDataSet(path string) [][]string {
	var array [][]string
	f, err := os.Open(path)
	if err != nil {
		log.Fatal(err)
	}

	scanner := bufio.NewScanner((f))

	for scanner.Scan() {
		line := scanner.Text()
		data := strings.Split(line, "|")
		array = append(array, data)
	}
	if err := scanner.Err(); err != nil {
		log.Fatal(err)
	}

	f.Close()

	return array
}
