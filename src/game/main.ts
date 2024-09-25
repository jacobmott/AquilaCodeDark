import { Boot } from "./scenes/Boot";
import { GameOver } from "./scenes/GameOver";
import { GameScene as MainGame } from "./scenes/Game";
import { MainMenu } from "./scenes/MainMenu";
import { AUTO, Game, WEBGL } from "phaser";
import { Preloader } from "./scenes/Preloader";
import { SpinePlugin } from "@esotericsoftware/spine-phaser";

//  Find out more information about the Game Config at:
//  https://newdocs.phaser.io/docs/3.70.0/Phaser.Types.Core.GameConfig
const config: Phaser.Types.Core.GameConfig = {
  type: AUTO,
  width: 1920,
  height: 1080,
  parent: "game-container",
  backgroundColor: "#4488aa",
  transparent: false,
  scene: [Boot, Preloader, MainMenu, MainGame, GameOver],
  // physics: {
  //   default: "matter",
  //   matter: {
  //     gravity: {
  //       y: 0,
  //       x: 0,
  //     },
  //     debug: {
  //       showBody: true,
  //       showStaticBody: true,
  //     },
  //     //If you turn on this sleeping.. it affects collision when gravity is off
  //     //enableSleeping: true,
  //   },
  // },
  plugins: {
    scene: [
      {
        key: "spine.SpinePlugin",
        plugin: SpinePlugin,
        mapping: "spine",
      },
    ],
  },
  fps: {
    forceSetTimeOut: true,
    // panicMax: 0,
    // smoothStep: false,
    target: 60,
  },
  scale: {
    mode: Phaser.Scale.FIT,
    autoCenter: Phaser.Scale.CENTER_BOTH,
  },
};

const StartGame = (parent: string) => {
  return new Game({ ...config, parent });
};

export default StartGame;
