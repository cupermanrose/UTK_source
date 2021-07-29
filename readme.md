# UTK description
__UTK(uncertain top-k query)__ is a query that given uncertain prefereces, it will reports all options that may belong to the top-k set.


This is the source code of [a], copying, and distributing it without premission from authors are not allowed.

PVLDB Reference Format:
[a] Kyriakos Mouratidis and Bo Tang. Exact Processing of Uncertain Top-k Queries in Multi-criteria Settings. PVLDB, 11 (8): 866-879, 2018.
DOI: https://doi.org/10.14778/3204028.3204031

# Auto Build

## Dependency

You should have installed one of
- yum
- apt-get
- brew

and __wget__ to use deplore UTK.

If you do not have __wget__ yet, use one of (which is dependent on your system)
- sudo yum install wget
- sudo apt-get install wget
- brew install wget

to install it.

Windows is temporarily not in the plan of release.

## Build 
```bash
wget https://raw.githubusercontent.com/BorisChenCZY/UTK/master/install.sh; sh install.sh
```

# Manual Build

You neet to have __qhull__ and __lp\_solve__ installed. It's depend on your system to use whether
- sudo yum install qhull-devel lp-solve
- sudo apt-get install qhull-bin lpsolve
- brew install qhull lp_solve
in bash/zsh to install it.

When finished, check 
```bash
which qhull
```
in bash/zsh is a /path/to/your/qhull to verify the installation.

After that, you need to check the official website of lp_solve to compile the API version of your system, which is `liblpsolve55.a`. Put it in the ./src folder, then ss
```bash
make
```

After the /UTK/bin folder is made, the __UTK__ in which is what we want.
