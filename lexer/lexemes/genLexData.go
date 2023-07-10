package main

import (
	"flag"
	"os"
	"path/filepath"
	"text/template"
)

type lexGenCtx struct {
	Package      string
	ConstLexemes []LexemeMeta
	VarLexemes   []LexemeMeta

	TokNames []string
}

func collectTokNames(ctx *lexGenCtx) {
	for _, v := range ctx.ConstLexemes {
		ctx.TokNames = append(ctx.TokNames, v.TokenName)
	}
	for _, v := range ctx.VarLexemes {
		ctx.TokNames = append(ctx.TokNames, v.TokenName)
	}
}

func main() {
	// a := 2
	// if a || 2 {

	// }
	tmplDir := flag.String("tmplDir", "", "")
	headerName := flag.String("hdrName", "", "")
	srcName := flag.String("srcName", "", "")
	flag.Parse()

	cppTmpl, err := template.ParseFiles(filepath.Join(*tmplDir, "lexemes.cpp.tmpl"))
	if err != nil {
		panic(err)
	}

	hppTmpl, err := template.ParseFiles(filepath.Join(*tmplDir, "lexemes.hpp.tmpl"))
	if err != nil {
		panic(err)
	}

	ctx := lexGenCtx{
		ConstLexemes: ConstantLexemes,
		VarLexemes:   VariableLexemes,
	}
	collectTokNames(&ctx)

	cppOut, err := os.Create(*srcName)
	if err != nil {
		panic(err)
	}
	defer cppOut.Close()

	hppOut, err := os.Create(*headerName)
	if err != nil {
		panic(err)
	}
	defer hppOut.Close()

	err = cppTmpl.Execute(cppOut, &ctx)
	if err != nil {
		panic(err)
	}

	err = hppTmpl.Execute(hppOut, &ctx)
	if err != nil {
		panic(err)
	}
}
