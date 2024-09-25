import { ComponentFixture, TestBed } from "@angular/core/testing";
import { TopNavLightSliderComponent } from "./top-nav-light-slider.component";

describe("TopNavLightSliderComponent", () => {
  let component: TopNavLightSliderComponent;
  let fixture: ComponentFixture<TopNavLightSliderComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [TopNavLightSliderComponent],
    }).compileComponents();

    fixture = TestBed.createComponent(TopNavLightSliderComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it("should create", () => {
    expect(component).toBeTruthy();
  });
});
