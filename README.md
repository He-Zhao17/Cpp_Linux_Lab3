# CSV Query Engine for Census Analysis

After living on your remote island for a while, you begin to long for a "normal" life once again. Sure, you now own a massive amount of bitcoin, but what good is it if your only friend is a seagull named Charles? The island is too deserted to be exciting, there aren't any good food options, and Charles tends to cheat at poker. You decide it's time to move on.

You buy a fake identity on the black market and decide to head back to the US for a while to see if anyone has started wearing masks yet. Ideally, you should probably use some of your bitcoin wealth to invest in real estate, but *where* should you buy? That truly is the question.

You decide to analyze some data from the US Census Bureau to find a perfect location to lay low for a while. Since the island only has a single TRS-80 and a dial-up internet connection, you get right to work writing the software in C.

## Building the Query System

We'll use data from the Census Bureau's American Community Survey (ACS) [Public Use Microdata Sample (PUMS)](https://www.census.gov/programs-surveys/acs/microdata.html). The dataset is in `.csv` format -- comma-separated values, with several types of information about each US state.

Your job is to read the files (from `stdin`) and filter them based on a user-provided query. For example:

A query of `NOC > 3` would search for PUMS records where the NOC (number of children in household) is greater than 3. Your query engine should support:

* Greater than (`>`)
* Less than (`<`)
* Equality (`=`)

And you should be able to search for any column in the CSV header (household income, number of vehicles, etc.).

The program you write will take a valid CSV as input and produce a valid CSV as its output. Therefore, you could chain multiple invocations of your program to "drill down" through the data with multiple queries. 

Here's a sample run:

```
# Find all the households with only one child:
./query 'NOC = 1' < ./pums_dataset.csv

(many lines of output are displayed)

# Find single-child households with more than 3 vehicles:
cat ./pums_dataset.csv | ./query 'NOC=1' | ./query 'VEH > 3'

(many lines, but less than the previous, are displayed)
```

You will need to experiment with the dataset to find meaningful information, so the [technical documentation](https://www.census.gov/programs-surveys/acs/technical-documentation/pums/documentation.html) will probably help.

## Dataset

A limited subset of the PUMS data is stored in the `./data` directory. You can find the complete set online at https://www.census.gov/programs-surveys/acs/microdata/access.html


## Restrictions

Our goal is to learn more about how C strings work at a low level. `strtok` and `strsep` would normally be good choices to tokenize the input files, but also have some pitfalls we will discuss in class. Instead, you should build your own tokenization functionality with loops or `strspn` / `strcspn`. You'll use these functions to extract the data between comma (`,`) characters in the CSV.


## Testing Your Code

As usual, you can run `make test` to run all the test cases, or `make test run=X` to run a specific test case, where `X` is the test case number.

