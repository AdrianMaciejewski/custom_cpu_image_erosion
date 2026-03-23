import chisel3._
import chiseltest._
import org.scalatest.flatspec.AnyFlatSpec

class ALUTester extends AnyFlatSpec with ChiselScalatestTester {

  private def doOp(
                    c: ALU,
                    op: AluOp.Type,
                    a: BigInt,
                    b: BigInt,
                    expRes: BigInt,
                    expOv: Boolean,
                    expBool: Boolean = false
                  ): Unit = {
    c.io.val1.poke(a.U)
    c.io.val2.poke(b.U)
    c.io.opCode.poke(op)

    // combinational in this ALU, step once anyway
    c.clock.step()

    c.io.result.expect(expRes.U)
    c.io.isOverflow.expect(expOv.B)
    c.io.isBooleanOpTrue.expect(expBool.B)
  }

  "ADD: no overflow and with overflow" should "pass" in {
    test(new ALU) { c =>
      // 10 + 20 = 30, no overflow
      doOp(c, AluOp.add, 10, 20, 30, expOv = false)

      // 0xFFFF_FFFF + 1 => result 0, carry=1 => overflow
      doOp(c, AluOp.add, BigInt("FFFFFFFF", 16), 1, 0, expOv = true)
    }
  }

  "MUL: no overflow and with overflow" should "pass" in {
    test(new ALU) { c =>
      // 3 * 7 = 21, fits in 32 bits
      doOp(c, AluOp.mul, 3, 7, 21, expOv = false)

      // 0x0001_0000 * 0x0001_0000 = 0x0001_0000_0000 (upper bits set) -> overflow
      val a = BigInt("00010000", 16) // 65536
      val b = BigInt("00010000", 16)
      val full = a * b               // 4294967296
      val lo32 = full & ((BigInt(1) << 32) - 1)
      doOp(c, AluOp.mul, a, b, lo32, expOv = true)
    }
  }

  "SUB: normal and underflow (borrow)" should "pass" in {
    test(new ALU) { c =>
      // 9 - 4 = 5, no underflow
      doOp(c, AluOp.sub, 9, 4, 5, expOv = false)

      // 3 - 5 -> wraps to 0xFFFF_FFFE, borrow => overflow flag true per our design
      val res = (BigInt(3) - BigInt(5)) & ((BigInt(1) << 32) - 1)
      doOp(c, AluOp.sub, 3, 5, res, expOv = true)
    }
  }

  "NEQ: inequality checks and result encoding" should "pass" in {
    test(new ALU) { c =>
      // equal -> false, result 0
      doOp(c, AluOp.neq, 123, 123, 0, expOv = false, expBool = false)

      // not equal -> true, result 1
      doOp(c, AluOp.neq, 123, 124, 1, expOv = false, expBool = true)

      // edge: different large values
      doOp(c, AluOp.neq, BigInt("FFFFFFFF", 16), 0, 1, expOv = false, expBool = true)
    }
  }

  "NEQ: equality checks and result encoding" should "pass" in {
    test(new ALU) { c =>
      // equal -> true, result 1
      doOp(c, AluOp.eql, 123, 123, 1, expOv = false, expBool = true)

      // not equal -> false, result 0
      doOp(c, AluOp.eql, 123, 124, 0, expOv = false, expBool = false)

      // edge: different large values
      doOp(c, AluOp.eql, BigInt("FFFFFFFF", 16), BigInt("FFFFFFFF", 16), 1, expOv = false, expBool = true)
    }
  }

  "NEQ: greater than or equal checks and result encoding" should "pass" in {
    test(new ALU) { c =>
      // greater than -> true, result 1
      doOp(c, AluOp.geq, 124, 123, 1, expOv = false, expBool = true)

      // equal -> true, result 0
      doOp(c, AluOp.geq, 123, 123, 1, expOv = false, expBool = true)

      // less than -> false, result 0
      doOp(c, AluOp.geq, 122, 123, 0, expOv = false, expBool = false)

      // edge: different large values
      doOp(c, AluOp.geq, BigInt("FFFFFFFF", 16), BigInt("FFFFFFFF", 16), 1, expOv = false, expBool = true)
    }
  }

  "Randomized ADD spot-checks (no reference to carry beyond 32 bits)" should "pass" in {
    test(new ALU) { c =>
      val rnd = new scala.util.Random(0xBEEFL)
      for (_ <- 0 until 100) {
        val a = BigInt(32, rnd)
        val b = BigInt(32, rnd)
        val full = a + b
        val lo32 = full & ((BigInt(1) << 32) - 1)
        val ov   = (full >> 32) != 0
        doOp(c, AluOp.add, a, b, lo32, expOv = ov)
      }
    }
  }
}

