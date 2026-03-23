import chisel3._
import chisel3.util._

class CPUTop extends Module {
  val io = IO(new Bundle {
    val done = Output(Bool ())
    val run = Input(Bool ())
    //This signals are used by the tester for loading and dumping the memory content, do not touch
    val testerDataMemEnable = Input(Bool ())
    val testerDataMemAddress = Input(UInt (16.W))
    val testerDataMemDataRead = Output(UInt (32.W))
    val testerDataMemWriteEnable = Input(Bool ())
    val testerDataMemDataWrite = Input(UInt (32.W))
    //This signals are used by the tester for loading and dumping the memory content, do not touch
    val testerProgMemEnable = Input(Bool ())
    val testerProgMemAddress = Input(UInt (16.W))
    val testerProgMemDataRead = Output(UInt (32.W))
    val testerProgMemWriteEnable = Input(Bool ())
    val testerProgMemDataWrite = Input(UInt (32.W))
  })

  //Creating components
  val programCounter = Module(new ProgramCounter())
  val dataMemory = Module(new DataMemory())
  val programMemory = Module(new ProgramMemory())
  val registerFile = Module(new RegisterFile())
  val controlUnit = Module(new ControlUnit())
  val alu = Module(new ALU())

  //Connecting the modules
  io.done := controlUnit.io.stop

  programCounter.io.run := io.run
  programCounter.io.stop := controlUnit.io.stop
  programCounter.io.jump := controlUnit.io.branch && alu.io.isBooleanOpTrue
  programCounter.io.programCounterJump := programMemory.io.instructionRead(15, 0)

  programMemory.io.address := programCounter.io.programCounter

  controlUnit.io.opcode := programMemory.io.instructionRead(31, 28)

  registerFile.io.aSel := programMemory.io.instructionRead(23, 20)
  registerFile.io.bSel := programMemory.io.instructionRead(19, 16)
  registerFile.io.writeSel := programMemory.io.instructionRead(27, 24)
  registerFile.io.writeEnable := controlUnit.io.registerWrite
  registerFile.io.writeData := Mux(controlUnit.io.dataReadEnable, dataMemory.io.dataRead, alu.io.result)

  alu.io.val1 := registerFile.io.a
  val ext = Wire(UInt(32.W))
  ext := programMemory.io.instructionRead(15, 0)
  alu.io.val2 := Mux(controlUnit.io.aluUsingImm, ext, registerFile.io.b)
  alu.io.opCode := controlUnit.io.aluOp

  dataMemory.io.address := registerFile.io.a
  dataMemory.io.writeEnable := controlUnit.io.dataWriteEnable
  dataMemory.io.dataWrite := registerFile.io.b


  //This signals are used by the tester for loading the program to the program memory, do not touch
  programMemory.io.testerAddress := io.testerProgMemAddress
  io.testerProgMemDataRead := programMemory.io.testerDataRead
  programMemory.io.testerDataWrite := io.testerProgMemDataWrite
  programMemory.io.testerEnable := io.testerProgMemEnable
  programMemory.io.testerWriteEnable := io.testerProgMemWriteEnable
  //This signals are used by the tester for loading and dumping the data memory content, do not touch
  dataMemory.io.testerAddress := io.testerDataMemAddress
  io.testerDataMemDataRead := dataMemory.io.testerDataRead
  dataMemory.io.testerDataWrite := io.testerDataMemDataWrite
  dataMemory.io.testerEnable := io.testerDataMemEnable
  dataMemory.io.testerWriteEnable := io.testerDataMemWriteEnable
}