import unittest
import HW6
class TestHW6(unittest.TestCase):

    def test_111(self):
        self.assertEqual(HW6.solve([1,1,1,1,1,1]), 1)

    def test_123(self):
        self.assertEqual(HW6.solve([1,2,3]), 3)

    def test_2(self):
        self.assertEqual(HW6.solve([3,4,5,6]), 6)

    def test_3(self):
        self.assertEqual(HW6.solve([1,4,3,9,1,2,4,10]), 10)
if __name__ == '__main__':
    unittest.main()

