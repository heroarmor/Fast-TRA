COMPILER = g++ -Wall -pthread
OBJ_DIR = obj
SRC_DIR = srcs
HEADERS = ftra.h channel_name.h
RUN_MESH_ST_DEPEND = mesh_cdg.o ftra.o utils.o run_mesh_st.o graph.o
RUN_MESH_MT_DEPEND = mesh_cdg.o ftra.o utils.o run_mesh_mt.o graph.o thread_pool.o
RUN_ROUTE_TABLE_GEN_DEPEND = mesh_cdg.o ftra.o utils.o route_table_gen.o graph.o thread_pool.o
RUN_SIMULATE_DEPEND = mesh_cdg.o ftra.o utils.o simulate.o graph.o thread_pool.o
vpath %.h $(SRC_DIR)

all : run_mesh_st run_mesh_mt route_table_gen simulator

run_mesh_st: $(addprefix $(OBJ_DIR)/, $(RUN_MESH_ST_DEPEND))
	$(COMPILER)  $^ -o $@

run_mesh_mt: $(addprefix $(OBJ_DIR)/, $(RUN_MESH_MT_DEPEND))
	$(COMPILER)  $^ -o $@

route_table_gen: $(addprefix $(OBJ_DIR)/, $(RUN_ROUTE_TABLE_GEN_DEPEND))
	$(COMPILER)  $^ -o $@

simulator : $(addprefix $(OBJ_DIR)/, $(RUN_SIMULATE_DEPEND))
	$(COMPILER)  $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	$(COMPILER) -c $< -o $@

clean:
	rm -f run_mesh_st run_mesh_mt route_table_gen simulator $(OBJ_DIR)/*.o

cleancp:
	rm -f checkpoints/*
