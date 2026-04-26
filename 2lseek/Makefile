# 2. 定义最终要生成的目标文件
TARGETS = make_sparse inspect_sparse

# 3. 伪目标 all：这是执行 make 时的默认入口
# 它告诉 make：我们要生成 TARGETS 里定义的所有东西
all: $(TARGETS)

make_sparse:make_sparse.cpp
	clang++ make_sparse.cpp -o make_sparse

inspect_sparse:inspect_sparse.cpp
	clang++ inspect_sparse.cpp -o inspect_sparse
