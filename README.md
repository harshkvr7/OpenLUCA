# OpenLUCA

A high-performance, asynchronous C++ Application for querying the NCBI Taxonomy database. Built with the Drogon web framework, OpenLUCA achieves sub-millisecond algorithmic latency and handles tens of thousands of requests per second, making it ideal for rapid taxonomic lineage and Lowest Universal Common Ancestor (LCA) resolution.

## Architecture Highlights

* **High-Speed Networking:** Built on Drogon's asynchronous, non-blocking event loop. This allows the API to handle tens of thousands of concurrent connections without slowing down.
* **Flat Memory Architecture:** Instead of using slow, scattered hash maps, the entire NCBI taxonomy tree is flattened into simple 1D arrays. This makes CPU data lookups lightning fast.
* **Optimized Search Algorithm:** Uses a "Binary Lifting" algorithm to find the Lowest Common Ancestor. Instead of climbing the tree one node at a time `O(H)`, it jumps exponentially `O(log H)` to find answers instantly.
* **Instant Startup:** The server parses the raw NCBI text files once, then saves the processed structure directly to disk as a binary cache. Subsequent server boots take less than a second.

## Build Instructions

### Prerequisites
* C++17 Compiler (GCC/Clang)
* CMake 3.15+
* [Drogon](https://github.com/drogonframework/drogon) & dependencies (`jsoncpp`, `uuid`)

### 1. Clone the Repository
First, bring the source code down to your local machine:
```bash
git clone [https://github.com/yourusername/OpenLUCA.git](https://github.com/yourusername/OpenLUCA.git)
cd OpenLUCA
```

### 2. Data Preparation
OpenLUCA requires the raw NCBI taxonomy data to build its internal trees.

From within the `OpenLUCA` root directory, download and extract the latest `taxdump.tar.gz` from the NCBI FTP server:

```bash
mkdir data && cd data
wget [https://ftp.ncbi.nlm.nih.gov/pub/taxonomy/taxdump.tar.gz](https://ftp.ncbi.nlm.nih.gov/pub/taxonomy/taxdump.tar.gz)
tar -zxvf taxdump.tar.gz nodes.dmp names.dmp
rm taxdump.tar.gz
cd ..
```

### 3. Compilation
Now, generate the build system and compile the binary:

```bash
# Generate build system
cmake -B build 

# Compile
cd build && make 
cd ..
```

### 4. Running the API
Once compiled and with the data/ directory populated, boot the server:

```bash
cd build
./TaxonomyBackend
```

Navigate to `http://localhost:8080` to view the monochrome web interface. (Note: The initial boot will take a few seconds to compute the binary lifting tables and generate the disk cache. Subsequent boots will be nearly instant).

## API Endpoints

* `GET /api/v1/lineage/{tax_id}` - Retrieve the taxonomic lineage of a species.
* `GET /api/v1/lca/{tax_id1}/{tax_id2}?algorithm=[naive|binary_lifting]` - Find the lowest common ancestor of two species.