@ [At] Nil
@@ [At2] Nil
@@@ [At3] Nil
Zero [Zero] 0
One [One] 1
bye {doBye}
de {doDe}
+ {doAdd}
- {doSub}
* {doMul}
let {doLet}
prinl {doPrin}
do {doDo}
setq {doSetq}
make {doMake}
link {doLink}
cons {doCons}
car {doCar}
cdr {doCdr}
quote {doQuote}
dump {doDump}
while {doWhile}
in {doIn}
out {doOut}
char {doChar}
line {doLine}
not {doNot}
for {doFor}
run {doRun}
hs {doHS}
= {doEq}
if {doIf}
call {doCall}
pack {doPack}
fork {doFork}
sleep {doSleep}
io {doIO}
load {doLoad}
eval {doEval}
mapcar {doMapcar}
sampleOpen {doSampleOpen}
sampleRead {doSampleRead}
bind {doBind}
listen {doListen}
skt {doSocket}
connect {doConnect}
http {doHTTP}
sktClose {doSocketClose}
loop {doLoop}
chop {doChop}
gc {doGC}
#######################################################################
############ TEST FUNCTIONS                              ##############
#######################################################################
G ((X)(+  1 2 X))
Z 10
ABCDEFGHIJ 10
ABCDEFGHIJK {doLongFunc}
ABCDEFGHIJABCDEFGHIJK {doVeryLongFunc}
