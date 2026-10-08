# docker-compose challenge; `pwnmake check` starts it, runs the solver, and tears
# it down. The solver needs `age`/`age-keygen` + python3-websocket (installed via
# repo-root prebuild.sh).
$(call ctf_check_tcp,$(DIR),24401,python3 solver/solve.py)
