import unittest
import mecab


class TestMecabTagger(unittest.TestCase):
    def test_get_dictionary_info(self):
        tagger = mecab.Tagger(mecab.get_model_args("./test-data/dic"))
        dic_infos = tagger.get_dictionary_info()
        del tagger

        self.assertEqual(len(dic_infos), 1)
        self.assertEqual(type(dic_infos), list)
        self.assertEqual(dic_infos[0].filename, "./test-data/dic/sys.dic")
        self.assertEqual(dic_infos[0].charset, "UTF-8")
        self.assertEqual(dic_infos[0].version, 102)

    def test_parse_node_with_lattice(self):
        tagger = mecab.Tagger(mecab.get_model_args("./test-data/dic"))
        node = tagger.parse_node_with_lattice("シリーズ中、カンフーシーンが一番多い。")
        del tagger

        self.assertEqual(len(node), 10)
        self.assertEqual(type(node), list)
        self.assertTrue(node[0].feature.startswith("BOS/EOS"))
        self.assertEqual(node[0].surface, "")
        self.assertEqual(node[1].surface, "シリーズ")
        self.assertTrue(node[-1].feature.startswith("BOS/EOS"))

    def test_parse_nbest_with_lattice(self):
        tagger = mecab.Tagger(mecab.get_model_args("./test-data/dic"))
        node = tagger.parse_nbest_with_lattice("シリーズ中、カンフーシーンが一番多い。", 10)
        del tagger

        self.assertEqual(len(node), 10)
        self.assertEqual(len(node[0]), 10)
        self.assertEqual(type(node), list)
        self.assertEqual(type(node[0]), list)
        self.assertTrue(node[0][0].feature.startswith("BOS/EOS"))
        self.assertEqual(node[0][0].surface, "")
        self.assertEqual(node[0][1].surface, "シリーズ")
        self.assertTrue(node[0][-1].feature.startswith("BOS/EOS"))


if __name__ == "__main__":
    unittest.main()
