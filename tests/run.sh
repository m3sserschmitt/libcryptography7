DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR && ./gen_test_keys.sh
cd $DIR/../build && ./libcryptography_wrapper_test
