# docker-compose challenge; `pwnmake check` starts it, runs the solver, and tears
# it down. The solver needs `age`/`age-keygen` + python3-websocket (installed via
# repo-root prebuild.sh).
$(call ctf_check_tcp,$(DIR),24401,python3 solver/solve.py)

# Served as a web host via the smart proxy; ctf_check_tcp doesn't auto-emit, so
# publish the nginx snippet directly.
$(call nginx_service,doubly-secure.ctf.hackucf.org,24401)
