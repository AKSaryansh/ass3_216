all:
	g++ -o cache_simulate a.cpp 
	./cache_simulate 64 1024 2 65536 8 memory_trace_files/trace7.txt > o1.txt
	

clean:
	rm ./cache_simulate