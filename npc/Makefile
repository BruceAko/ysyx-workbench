all:
	@echo "Write this Makefile by your self."

sim:
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	verilator --cc --exe --build -j 0 -Wall ./csrc/main.cpp ./vsrc/top.v
	obj_dir/Vtop

run:
	obj_dir/Vtop

include ../Makefile
