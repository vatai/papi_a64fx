import json
import sys


def main(key):
    data = json.load(sys.stdin)
    if key == "keys":
        for k in data.keys():
            print(k)
    else:
        print(data[key])


if __name__ == "__main__":
    main(sys.argv[1])
