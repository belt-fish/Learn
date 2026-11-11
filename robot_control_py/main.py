import sys

from robot import Control, Params, Type

def demo():
    ctrl = Control()
    ctrl.submit(Type.FORWARD, Params.cm(100))
    ctrl.submit(Type.ROTATE, Params.deg(90))
    ctrl.submit(Type.WAVE, Params.n_times(3))
    ctrl.run()

def demo_chain():
    (
        Control()
        .submit(Type.FORWARD, Params.cm(50))
        .submit(Type.WAVE, Params.n_times(1))
        .run()
    )

def demo_error():
    try:
        Control().submit(Type.FORWARD, Params.cm(0))
    except ValueError as e:
        print(f"参数校验生效：{e}")

def main(argv):
    if "--list" in argv:
        print("支持的动作：")
        for t in Type:
            print(f"  {t.value}")
        return 0

    demo()
    print()
    demo_chain()
    print()
    demo_error()
    return 0

if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
