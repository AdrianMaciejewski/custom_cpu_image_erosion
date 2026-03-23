import chisel3._
import chiseltest._
import org.scalatest.flatspec.AnyFlatSpec
import org.scalatest.matchers.should.Matchers



class ControlUnitTester extends AnyFlatSpec with ChiselScalatestTester with Matchers {

  private case class V(
                        opcode: Int,
                        aluOp: AluOp.Type,
                        aluUsingImm: Boolean,
                        stop: Boolean,
                        branch: Boolean,
                        dataReadEnable: Boolean,
                        dataWriteEnable: Boolean,
                        registerWrite: Boolean
                      )

  private val vectors = Seq(
    V(1,  AluOp.add, false, false, false, false, false, true),  // add
    V(2,  AluOp.sub, false, false, false, false, false, true),  // sub
    V(3,  AluOp.add, true,  false, false, false, false, true),  // addi
    V(4,  AluOp.mul, true,  false, false, false, false, true),  // muli
    V(5,  AluOp.add, false, false, false, false, true,  false), // store
    V(6,  AluOp.add, false, false, false, true,  false, true),  // load
    V(9,  AluOp.eql, false, false, true,  false, false, false), // beq
    V(10, AluOp.neq, false, false, true,  false, false, false), // bne
    V(11, AluOp.geq, false, false, true,  false, false, false), // bge
    V(15, AluOp.add, false, true,  false, false, false, false)  // exit
  )

  behavior of "ControlUnit"

  it should "decode known opcodes into the correct control signals" in {
    test(new ControlUnit) { c =>
      for (v <- vectors) {
        c.io.opcode.poke(v.opcode.U)

        c.clock.step()

        c.io.aluOp.expect(v.aluOp)
        c.io.aluUsingImm.expect(v.aluUsingImm.B)
        c.io.stop.expect(v.stop.B)
        c.io.branch.expect(v.branch.B)
        c.io.dataReadEnable.expect(v.dataReadEnable.B)
        c.io.dataWriteEnable.expect(v.dataWriteEnable.B)
        c.io.registerWrite.expect(v.registerWrite.B)
      }
    }
  }
}
