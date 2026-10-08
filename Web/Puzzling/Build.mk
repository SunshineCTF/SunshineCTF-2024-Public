# docker-compose web challenge; `pwnmake check` starts it, runs the solver, and
# tears it down.
$(call ctf_check_web,$(DIR),24304,puzzling.ctf.hackucf.org,python3 solution/solve.py)
