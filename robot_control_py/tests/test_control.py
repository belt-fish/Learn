import os
import sys
import unittest

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from robot import Control, Params, Type

class TestControl(unittest.TestCase):
    def test_submit_count(self):
        c = Control()
        c.submit(Type.FORWARD, Params.cm(10))
        c.submit(Type.ROTATE, Params.deg(45))
        self.assertEqual(c.count, 2)

    def test_action_text(self):
        c = Control()
        c.submit(Type.FORWARD, Params.cm(100))
        action = c._Control__actions[0]
        self.assertEqual(action.name(), "前进")
        self.assertIn("100", action.params())
        self.assertIn("100 cm", action.execute())

    def test_zero_distance_raises(self):
        with self.assertRaises(ValueError):
            Control().submit(Type.FORWARD, Params.cm(0))

    def test_negative_degree_raises(self):
        with self.assertRaises(ValueError):
            Control().submit(Type.ROTATE, Params.deg(-5))

    def test_missing_param_raises(self):
        with self.assertRaises(ValueError):
            Control().submit(Type.ROTATE, Params.cm(10))

    def test_wrong_type_raises(self):
        with self.assertRaises(TypeError):
            Control().submit("forward", Params.cm(10))

    def test_params_immutable(self):
        p = Params.cm(100)
        with self.assertRaises(Exception):
            p.distance_cm = 200

if __name__ == "__main__":
    unittest.main()
