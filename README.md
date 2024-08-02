# FCC Software Website

Standard `github-pages`-site. All dependencies (`jquery`, `bootstrap`) are
included in the repository and any change will be automatically deployed to
<https://fccsw.web.cern.ch/>.


## Local Testing

Change content and for local testing, serve the page with

```sh
    bundle exec jekyll serve --baseurl=
```

and point your browser to `localhost:4000`.


## Deployment

### Development

To build the website run:
```sh
    bundle exec jekyll build --baseurl='/devel' --destination=<DEST>
```

> `<DEST>` is name of the directory into which the build site will be saved to

and upload the resulting directory:
```sh
rsync -avh <DEST>/ lxplus:/eos/project/f/fccsw-web/www/devel/ --delete
```

### Production

The FCC Software website is at the moment hosted by GitHub at
`https://hep-fcc.github.io/FCCSW/`, but can be reached using any of the
following URLs:
* `https://cern.ch/fccsw` -- redirect lives here: `/eos/project/f/fccsw-web/www/index.html`
* `https://fccsw.web.cern.ch/` -- redirect lives here: `/eos/project/f/fccsw-web/www/index.html`
* `https://hep-fcc.github.io/` -- redirect lives here: `https://github.com/HEP-FCC/hep-fcc.github.io/blob/main/index.html`
