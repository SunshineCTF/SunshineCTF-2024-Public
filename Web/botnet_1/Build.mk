# Flag 1. docker-compose web challenge; `pwnmake check` starts it, runs the
# solver, and tears it down.
$(call ctf_check_web,$(DIR),24301,botnet.ctf.hackucf.org,python3 solver.py)
