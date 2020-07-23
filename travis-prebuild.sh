function travis_skip()
{

  local platform_key=$1

  # grab all pde and ino example sketches
  local examples=$(find $PWD -name "*.ino")

  # loop through example sketches
  for example in $examples; do

    # store the full path to the example's sketch directory
    local example_dir=$(dirname $example)

    touch ${example_dir}/.${platform_key}.test.skip

  done

}

travis_skip uno
travis_skip zero # fails because of pgmspace.h
travis_skip leonardo
travis_skip m4
# travis_skip mega2560
# travis_skip esp32
# travis_skip esp8266
