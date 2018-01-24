# SMC2017

Replicable python code for the article:

```
@unpublished{Bayle2017,
  author = {Bayle, Yann and Robine, Matthias and Hanna, Pierre},
  link = {https://arxiv.org/abs/1706.07613},
  title = {Toward faultless content-based playlists generation for instrumentals},
  year = {2017}
}
```

### Details

**Task**: content-based musical playlists generation focused on Songs and Instrumentals.

**Musical Dataset**: [CCMixter](https://members.loria.fr/ALiutkus/kam/), [Jamendo](http://www.mathieuramona.com/wp/data/jamendo/), [MedleyDB](http://medleydb.weebly.com/) and [SATIN](https://www.researchgate.net/publication/317824409_SATIN_A_Persistent_Musical_Database_for_Music_Information_Retrieval). 

**Results**: Our suggested approach generates an Instrumental playlist with up to three times less false positives than state-of-the-art.

**Contributions**:
- The first review of SIC systems in the context of playlist generation.
- The first formal design of experiment of the Song Instrumental Classification (SIC) task.
- A demonstration that the use of frame features outperforms the use of global track features in the case of SIC and thus diminish the risk of an algorithm being a "[Horse](http://ieeexplore.ieee.org/abstract/document/6847693/)".
- A knowledge-based SIC algorithm ---easily explainable--- that can process large musical database whereas state-of-the-art algorithms cannot.
- A new track tagging method based on frame predictions that outperforms the Markov model in terms of accuracy and f-score.
- A demonstration that better playlists related to a tag can be generated when the autotagging algorithm focuses only on this tag.

### Requirements/Dependencies

- Python 2 and 3 environments because all scripts works under python 3 except for YAAFE that only works under Python 2
- You need at least 54Go of free space to store the data and audio features
- You need to manually download and store tracks in a dir named: tracks/
- [Yaafe](https://github.com/Yaafe/Yaafe)'s intallation: `conda install -c https://conda.anaconda.org/yaafe yaafe`
- [Marsyas](https://github.com/marsyas/marsyas/)'s installation: http://marsyas.info/doc/manual/marsyas-user/Step_002dby_002dstep-building-instructions.html#Step_002dby_002dstep-building-instructions
