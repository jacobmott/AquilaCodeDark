/** @type {import('tailwindcss').Config} */
const { join } = require("path");

module.exports = {
  content: [join(__dirname, "src/**/!(*.stories|*.spec).{ts,html}")],
  theme: {
    extend: {
      zIndex: {
        1: "1",
        2: "2",
        3: "3",
        4: "4",
        5: "5",
        6: "6",
        7: "7",
        8: "8",
        9: "9",
        11: "11",
        12: "12",
      },
      spacing: {
        "1/6": "16.666667%",
        "2/6": "33.333333%",
        "3/6": "50%",
        "4/6": "66.666667%",
        "5/6": "83.333333%",
      },
      transitionProperty: {
        width: "width",
        height: "height",
      },
      screens: {
        tall: { raw: "(min-height: 800px)" },
        // => @media (min-height: 800px) { ... }
      },
      backgroundSize: {
        "size-200": "200% 200%",
      },
      backgroundPosition: {
        "pos-0": "0% 0%",
        "pos-100": "100% 100%",
      },
      backgroundImage: {
        "gradient-radial": "radial-gradient(var(--tw-gradient-stops))",
      },
      colors: {
        aquila: {
          50: "#2B2F53",
          400: "#3c396a",
          900: "#1D1C34",
        },
        aquilapink: {
          50: "#B583E6",
          100: "#A768E6",
          400: "#9B51E6",
          600: "#9340E6",
          900: "#8E37E6",
        },
      },
      fontSize: {
        sssm: ["0.5rem", "0.875rem"], // 8px, 14px
        ssm: ["0.625rem", "1rem"], // 10px, 16px
      },
    },
  },
  plugins: [],
};
