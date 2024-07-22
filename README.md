

# havoc-memfiles

<p align="center">
<img src="https://raw.githubusercontent.com/Sh4N4C1/gitbook/main/images/memfiles1.png" alt="memfiles">
</p>

All work by the talented [Octoberfest7](https://github.com/Octoberfest7)

This project is basically based on [memFiles](https://github.com/Octoberfest7/memFiles)

Details are all at [memFiles](https://github.com/Octoberfest7/memFiles), I'm just trying to port to Havoc c&c.

Note that in my experiments it can be extremely *unstable*, and i changed `redteam` str to `Temp`, which means the fake file path must contain Temp.

## Install

```bash
git clone https://github.com/Octoberfest7/memFiles
make

meminit
...
memdumpclean
```
