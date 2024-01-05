# 24nov16abu
# (c) Software Lab. Alexander Burger

### Main Entry Point ###
main [Main] NIL

### System Globals ###
@ [At] NIL
@@ [At2] NIL
@@@ [At3] NIL
This [This] NIL
meth [Meth] {doMeth}
*Dbg [Dbg] NIL
*Scl [Scl] 0
*Class [Class] NIL
^ [Up] NIL
*Err [Err] NIL
*Msg [Msg] NIL
*Bye [Bye] NIL  # Last unremovable symbol

### System Functions ###
abs {doAbs}
+ {doAdd}
all {doAll}
and {doAnd}
any {doAny}
append {doAppend}
,((((NIL @X @X)) (((@A . @X) @Y (@A . @Z)) (append @X @Y @Z))) . T)
apply {doApply}
arg {doArg}
args {doArgs}
argv {doArgv}
-> {doArrow}
as {doAs}
asoq {doAsoq}
assoc {doAssoc}
at {doAt}
atom {doAtom}
bind {doBind}
& {doBitAnd}
| {doBitOr}
bit? {doBitQ}
x| {doBitXor}
bool {doBool}
box {doBox}
box? {doBoxQ}
! {doBreak}
by {doBy}
bye {doBye}
caaar {doCaaar}
caadr {doCaadr}
caar {doCaar}
cadar {doCadar}
cadddr {doCadddr}
caddr {doCaddr}
cadr {doCadr}
car {doCar}
case {doCase}
casq {doCasq}
catch {doCatch}
cdaar {doCdaar}
cdadr {doCdadr}
cdar {doCdar}
cddar {doCddar}
cddddr {doCddddr}
cdddr {doCdddr}
cddr {doCddr}
cdr {doCdr}
char {doChar}
chain {doChain}
chop {doChop}
circ {doCirc}
circ? {doCircQ}
clip {doClip}
cmd {doCmd}
cnt {doCnt}
: {doCol}
con {doCon}
conc {doConc}
cond {doCond}
cons {doCons}
copy {doCopy}
cut {doCut}
date {doDate}
de {doDe}
dec {doDec}
def {doDef}
default {doDefault}
del {doDel}
delete {doDelete}
,((((@A (@A . @Z) @Z)) ((@A (@X . @Y) (@X . @Z)) (delete @A @Y @Z))) . T)
delq {doDelq}
diff {doDiff}
/ {doDiv}
dm {doDm}
do {doDo}
e {doE}
env {doEnv}
eof {doEof}
eol {doEol}
== {doEq}
=0 {doEq0}
=1 {doEq1}
=T {doEqT}
= {doEqual}
eval {doEval}
extra {doExtra}
extract {doExtract}
fifo {doFifo}
fill {doFill}
filter {doFilter}
fin {doFin}
finally {doFinally}
find {doFind}
fish {doFish}
flg? {doFlgQ}
flip {doFlip}
flush {doFlush}
fold {doFold}
for {doFor}
,((((@N @End) (for @N 1 @End 1)) ((@N @Beg @End) (for @N @Beg @End 1)) ((@N @Beg @End @Step) (equal @N @Beg)) ((@N @Beg @End @Step) (^ @I (box (-> @Beg))) (_for @N @I @End @Step))) . T)
format {doFormat}
from {doFrom}
full {doFull}
fully {doFully}
fun? {doFunQ}
gc {doGc}
>= {doGe}
ge0 {doGe0}
get {doGet}
getl {doGetl}
glue {doGlue}
> {doGt}
gt0 {doGt0}
head {doHead}
heap {doHeap}
==== {doHide}
idx {doIdx}
if {doIf}
if2 {doIf2}
ifn {doIfn}
in {doIn}
inc {doInc}
index {doIndex}
intern {doIntern}
isa {doIsa}
job {doJob}
last {doLast}
<= {doLe}
le0 {doLe0}
length {doLength}
let {doLet}
let? {doLetQ}
line {doLine}
link {doLink}
list {doList}
lit {doLit}
lst? {doLstQ}
load {doLoad}
loop {doLoop}
low? {doLowQ}
lowc {doLowc}
< {doLt}
lt0 {doLt0}
lup {doLup}
made {doMade}
make {doMake}
map {doMap}
,((((@V . @L) (^ @Lst (box (apply get (-> @L)))) (_map @V @Lst))) . T)
mapc {doMapc}
mapcan {doMapcan}
mapcar {doMapcar}
mapcon {doMapcon}
maplist {doMaplist}
maps {doMaps}
match {doMatch}
max {doMax}
maxi {doMaxi}
member {doMember}
,((((@X (@X . @))) ((@X (@ . @Y)) (member @X @Y))) . T)
memq {doMemq}
meta {doMeta}
method {doMethod}
min {doMin}
mini {doMini}
mix {doMix}
mmeq {doMmeq}
* {doMul}
*/ {doMulDiv}
name {doName}
nand {doNand}
n== {doNEq}
n0 {doNEq0}
nT {doNEqT}
<> {doNEqual}
need {doNeed}
new {doNew}
next {doNext}
nil {doNil}
,((((@X) (^ @ (not (-> @X))))) . T)
nond {doNond}
nor {doNor}
not {doNot}
,(((@P (1 (-> @P)) T (fail)) (@P)) . T)
nth {doNth}
num? {doNumQ}
off {doOff}
offset {doOffset}
on {doOn}
one {doOne}
onOff {doOnOff}
opt {doOpt}
or {doOr}
,(((@L (^ @C (box (-> @L))) (_or @C))) . T)
out {doOut}
pack {doPack}
pair {doPair}
pass {doPass}
path {doPath}
pat? {doPatQ}
peek {doPeek}
pick {doPick}
pop {doPop}
++ {doPopq}
pre? {doPreQ}
prin {doPrin}
prinl {doPrinl}
print {doPrint}
println {doPrintln}
printsp {doPrintsp}
prior {doPrior}
prog {doProg}
prog1 {doProg1}
prog2 {doProg2}
prop {doProp}
:: {doPropCol}
prove {doProve}
push {doPush}
push1 {doPush1}
push1q {doPush1q}
put {doPut}
putl {doPutl}
queue {doQueue}
quit {doQuit}
rand {doRand}
range {doRange}
rank {doRank}
rassoc {doRassoc}
read {doRead}
% {doRem}
replace {doReplace}
rest {doRest}
reverse {doReverse}
rot {doRot}
run {doRun}
sect {doSect}
seed {doSeed}
seek {doSeek}
; {doSemicol}
send {doSend}
set {doSet}
=: {doSetCol}
setq {doSetq}
>> {doShift}
size {doSize}
skip {doSkip}
sort {doSort}
space {doSpace}
split {doSplit}
sp? {doSpQ}
sqrt {doSqrt}
state {doState}
stem {doStem}
str {doStr}
strip {doStrip}
str? {doStrQ}
- {doSub}
sum {doSum}
super {doSuper}
swap {doSwap}
sym {doSym}
sym? {doSymQ}
t {doT}
tail {doTail}
text {doText}
throw {doThrow}
till {doTill}
$ {doTrace}
trim {doTrim}
try {doTry}
type {doType}
unify {doUnify}
unless {doUnless}
until {doUntil}
up {doUp}
upp? {doUppQ}
uppc {doUppc}
use {doUse}
val {doVal}
,((((@V . @L) (^ @V (apply get (-> @L))) T)) . T)
when {doWhen}
while {doWhile}
with {doWith}
xchg {doXchg}
xor {doXor}
yoke {doYoke}
zap {doZap}
zero {doZero}
calloc {doCalloc}
free {doFree}
SDL_CreateWindow {LISP_SDL_CreateWindow}
SDL_CreateRenderer {LISP_SDL_CreateRenderer}
SDL_PollEvent {LISP_SDL_PollEvent}
SDL_DestroyWindow {LISP_SDL_DestroyWindow}
SDL_RenderDrawLine {LISP_SDL_RenderDrawLine}
SDL_RenderPresent {LISP_SDL_RenderPresent}
SDL_SetRenderDrawColor {LISP_SDL_SetRenderDrawColor}
SDL_GetMouseState {LISP_SDL_GetMouseState}
SDL_Init {LISP_SDL_Init}
SDL_Quit {LISP_SDL_Quit}
SDL_PushEvent {LISP_SDL_PushEvent}
COMP_PACK {COMP_PACK}
PACK {doPACK}
uv_loop {LISP_uv_loop}
uv_run_nowait {LISP_uv_run_nowait}
uv_queue_work {LISP_uv_queue_work}
uv_tcp_connect {LISP_uv_tcp_connect}
uv_tcp_write {LISP_uv_tcp_write}
uv_tcp_read {LISP_uv_tcp_read}
