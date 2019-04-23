# FCCSW website

Standard `github-pages`-site. All dependencies (`jquery`, `bootstrap-sass`) are included in the repository and any change should be automatically deployed to <http://hep-fcc.github.io/FCCSW/>


## Local testing

Change content and for local testing, serve the page with

```
    jekyll serve --baseurl=
```

and point your browser to `localhost:4000`.

§§§ Deploying

Build page with

```
    jekyll build --destination=DEST
```

For <cern.ch/fccsw> there is an associated eos folder `/eos/project/f/fccsw-web/www` (subscripe to e-group to gain write access)
Currently it just redirects to <hep-fcc.github.io/FCCSW> but could be used as DEST.
  



## Adding permalinks

Add url and name in `docpage/_data/permalinks.yml`

