// Run after compilation: node tests/simulate.cjs path/to/MIDI_SWITCH.ino.hex
// Dependency: npm install --no-save avr8js@0.21.1
const fs = require('node:fs');
const assert = require('node:assert/strict');
const avr = require(process.env.AVR8JS_PATH || 'avr8js');
const hex = fs.readFileSync(process.argv[2], 'utf8');
const flash = new Uint8Array(32768);
for (const line of hex.trim().split(/\r?\n/)) {
  const count = parseInt(line.slice(1, 3), 16);
  const address = parseInt(line.slice(3, 7), 16);
  if (line.slice(7, 9) !== '00') continue;
  for (let i = 0; i < count; i++) flash[address+i] = parseInt(line.slice(9+i*2, 11+i*2),16);
}
const cpu = new avr.CPU(new Uint16Array(flash.buffer));
new avr.AVRTimer(cpu, avr.timer0Config);
new avr.AVRTimer(cpu, avr.timer2Config);
const portB = new avr.AVRIOPort(cpu, avr.portBConfig);
new avr.AVRIOPort(cpu, avr.portDConfig);
const uart = new avr.AVRUSART(cpu, avr.usart0Config, 16000000);
let bytes = [], checks = 0, digitEdges = [0,0];
uart.onByteTransmit = b => bytes.push(b);
portB.setPin(3, true); portB.setPin(4, true);
portB.addListener((v,old) => {
  assert.notEqual(v & 6, 6, 'Both display anodes must never be active');
  for(let i=0;i<2;i++) if((v & (2<<i)) && !(old & (2<<i))) digitEdges[i]++;
});
function run(ms) {
  const end=cpu.cycles+Math.round(ms*16000);
  while(cpu.cycles<end) { avr.avrInstruction(cpu); cpu.tick(); }
}
function expect(expected,label) {
  assert.deepEqual(bytes,expected,label); bytes=[]; checks++; console.log('PASS '+label);
}
function press(which) { portB.setPin(which==='up'?4:3,false); }
function release(which) { portB.setPin(which==='up'?4:3,true); }
function tap(which,ms=160) { press(which); run(ms); release(which); run(60); }
const pc=n=>[0xC0,n,0xB0,11,127];
run(1100); expect(pc(0),'Startup preset 01 + FX on');
assert.equal(uart.baudRate,31250); checks++;
tap('up'); expect(pc(1),'UP -> 02, one PC and one CC');
tap('down'); expect(pc(0),'DOWN -> 01');
tap('down'); expect(pc(23),'Wrap 01 -> 24');
tap('up'); expect(pc(0),'Wrap 24 -> 01');
// Sub-debounce chatter must not select a preset.
for(let i=0;i<5;i++) { press('up'); run(4); release('up'); run(4); }
run(80); expect([],'Contact chatter ignored');
tap('up',45); expect(pc(1),'Short valid tap retained');
press('up'); run(200); expect(pc(2),'Held switch initial step');
run(400); expect([],'No early auto-repeat');
run(150); expect(pc(3),'Repeat after initial hold delay');
run(250); expect(pc(4),'Repeat interval');
release('up'); run(60); expect([],'Release stops repeat');
press('up'); run(30); press('down'); run(200);
expect([0xB0,11,0],'Both switches: bypass without preset change');
run(1000); expect([],'Holding both does not flood MIDI');
release('up'); run(150); expect([],'Partial release cannot select preset');
release('down'); run(60); tap('down'); expect(pc(3),'Next single press resumes FX');
press('down'); run(30); press('up'); run(180);
expect([0xB0,11,0],'Chord works in opposite order');
release('up');release('down');run(60);
// If the second switch arrives after a completed single action, suppress until release.
press('up');run(160);expect(pc(4),'Single action before late second switch');
press('down');run(1000);expect([],'Late chord suppressed until both released');
release('up');release('down');run(60);
for(let n=0;n<24;n++) { tap('up'); expect(pc((5+n)%24),'Full preset cycle '+(n+1)); }
assert(digitEdges[0]>1000 && digitEdges[1]>1000,'Display timer is active'); checks++;
console.log(`PASS: ${checks} checks; display edges ${digitEdges.join('/')}. No hardware test implied.`);
