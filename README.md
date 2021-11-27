# SimpleBlockchain
## Naudojimosi instrukcija:
### Kompiliavimas:
* Wsl, Linux arba macOS komandinėje eilutėje, nuėjus į katalogą kuriame būtų main.cpp ir parašyti make.
### Pasileidimas:
* Reikia parašyti ./main
### Programos rezultatai:
* Programa sukurs tris txt failus: "GeneratedTransactions.txt", kuriame bus visos sugeneruotos transakcijos, "BlockTransactions.txt", kuriame bus pateikiamos kiekviename bloke esančios transakcijos, ir "Blockchain.txt", kuriame bus informacija apie kiekvieną bloką, esantį blockchain'e.
### Papildomas programos funkcionalumas:
Paleidus programą su argumentu nuo 0 iki 100 imtinai, pvz., ./main 5 bus išvedama informacija į consolę/terminalą apie penktą bloką.
### Libbitcoin testavimas
* Testavau su 150000-to bloko transakcijų hash'ais
![image](https://user-images.githubusercontent.com/78591148/143686950-b410c2fd-7bdb-4599-8639-41f4bc522257.png)
* Gautas merkle root hash'as sutampa (Little Endian)